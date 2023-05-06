#include "header.h"
#include "message_queues.h"
#include "shared_data.h"
#include "shared_memory.h"
using namespace std;
sigset_t signal_set; // set of signals
vector<int> employee1, employee2;
pthread_mutex_t employee1_data_mutex;
pthread_mutex_t employee2_data_mutex;
pthread_cond_t employee1_cond;
pthread_cond_t employee2_cond;
int line_mq_id;
int line_8_sorageArea_mq_id;
void *employees(void *arg)
{
    int id = *((int *)arg);
    while (true)
    {
        if (check_mq_size(line_mq_id, 0))
        {
            if (id == 1)
            {
                pthread_mutex_lock(&employee1_data_mutex);
                long type = 0;
                int employee_1_recive = receive_Lines_mq(line_mq_id, type);
                employee1.push_back(employee_1_recive);
                edit_shared_memory(SHARED_MEMORY_OPERATION(EMPLOYEE1), employee1.size());
                if (employee1.size() >= 10)
                {
                    pthread_cond_signal(&employee1_cond);
                }
                pthread_mutex_unlock(&employee1_data_mutex);
            }
            else
            {
                pthread_mutex_lock(&employee2_data_mutex);
                long type = 0;
                int employee_2_recive = receive_Lines_mq(line_mq_id, type);
                employee2.push_back(employee_2_recive);
                edit_shared_memory(SHARED_MEMORY_OPERATION(EMPLOYEE2), employee1.size());
                if (employee1.size() >= 10)
                {
                    pthread_cond_signal(&employee2_cond);
                }
                pthread_mutex_unlock(&employee2_data_mutex);
            }
            usleep(100000);
        }
    }

    return NULL;
}

void *line_8_thread(void *arg)
{
    while (true)
    {
        if (employee1.size() >= 10 || employee2.size() >= 10)
        {
            pthread_mutex_lock(&employee1_data_mutex);
            while (employee1.size() < 10)
            {
                pthread_cond_wait(&employee1_cond, &employee1_data_mutex);
            }
            int type_a = 0, type_b = 0, type_c = 0;
            for (int i = 0; i < 10; i++)
            {
                int value = *(employee1.begin());
                if (value == 1)
                    type_a++;
                else if (value == 2)
                    type_b++;
                else
                    type_c++;
                send_line8_mq(type_a, type_b, type_c, line_8_sorageArea_mq_id);
                employee1.erase(employee1.begin());
            }
            edit_shared_memory(SHARED_MEMORY_OPERATION(IS_PRINTING_EXPIRED_DATE), 1);
            usleep(PRINT_EXPIRED_DATE_TIME * 100000);
            edit_shared_memory(SHARED_MEMORY_OPERATION(IS_PRINTING_EXPIRED_DATE), 0);
            pthread_mutex_unlock(&employee1_data_mutex);

            pthread_mutex_lock(&employee2_data_mutex);
            while (employee2.size() < 10)
            {
                pthread_cond_wait(&employee2_cond, &employee2_data_mutex);
            }
            for (int i = 0; i < 10; i++)
            {
                int value = *(employee2.begin());
                if (value == 1)
                    type_a++;
                else if (value == 2)
                    type_b++;
                else
                    type_c++;
                send_line8_mq(type_a, type_b, type_c, line_8_sorageArea_mq_id);
                employee2.erase(employee2.begin());
            }
            usleep(PRINT_EXPIRED_DATE_TIME * 100000);

            pthread_mutex_unlock(&employee2_data_mutex);
        }
    }

    return NULL;
}

void wait_for_starting_signal()
{

    // initialize the signal set
    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGUSR1);
    // block the signals in the set
    sigprocmask(SIG_BLOCK, &signal_set, nullptr);

    // code that does some work

    // wait for a signal
    int signal_number;
    sigwait(&signal_set, &signal_number);
}

void create_threads()
{
    pthread_t first_thread, second_thread, third_thread;
    int employee1_id = 1;
    int employee2_id = 2;
    // initialize mutex and condition variable objects
    pthread_mutex_init(&employee1_data_mutex, NULL);
    pthread_mutex_init(&employee2_data_mutex, NULL);
    pthread_cond_init(&employee1_cond, NULL);
    pthread_cond_init(&employee2_cond, NULL);
    // create threads
    pthread_create(&first_thread, NULL, employees, (void *)&employee1_id);
    pthread_create(&second_thread, NULL, employees, (void *)&employee2_id);
    pthread_create(&third_thread, NULL, line_8_thread, NULL);
    // join threads
    pthread_join(first_thread, NULL);
    pthread_join(second_thread, NULL);
    pthread_join(third_thread, NULL);
}

int main(int arc, char *argv[])
{
    if (arc != 3)
    {
        cout << "usage: ./line_8 <line_mq_id> <line_8_sorageArea_mq_id>" << endl;
        return 1;
    }
    line_mq_id = atoi(argv[1]);
    line_8_sorageArea_mq_id = atoi(argv[2]);

    wait_for_starting_signal();

    create_threads();
    return 0;
}
