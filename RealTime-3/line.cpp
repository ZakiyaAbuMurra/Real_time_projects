#include "shared_data.h"
#include "message_queues.h"
#include "header.h"
#include "shared_memory.h"
using namespace std;
int x = 0;
pthread_mutex_t mutex_s;
pthread_cond_t cond;
sigset_t signal_set; // set of signals
bool first_seq_threads_done = false;
bool running = true;
int num_threads;
int num_seq_threads;
int sleep_time;
int num_of_pices = 0;
int mq_id;
int current_thread = 1;
int threads_count = 0;

string type = "";
void *thread_function(void *id)
{
    int thread_id = *((int *)id);
    int local_var = 0;
    while (true)
    {
        // check if to stop the production lines
        if (read_shared_memory(SHARED_MEMORY_OPERATION(STOP_PRODUCTION_LINES)))
        {
            // wait until can continue
            while (read_shared_memory(SHARED_MEMORY_OPERATION(STORAGE_AREA)) > STORAGE_AREA_MIN_CAPACITY)
            {
                usleep(1000);
            }
        }

        pthread_mutex_lock(&mutex_s);
        if (threads_count >= num_threads)
        {
            int t; // represt the type of the message as an integer
            if (type == "A")
                t = 1;
            else if (type == "B")
                t = 2;
            else if (type == "C")
                t = 3;
            // send the pice to the next stage
            usleep(sleep_time * 100000);
            send_lines_mq(t, mq_id);
        }
        else
        {
            if (thread_id <= num_seq_threads && thread_id == current_thread)
            {

                current_thread++;
                threads_count++;
                if (current_thread == num_seq_threads)
                {
                    first_seq_threads_done = true;
                    pthread_cond_signal(&cond);
                }
                usleep(sleep_time * 100000);
            }
            else
            {
                while (!first_seq_threads_done)
                {
                    pthread_cond_wait(&cond, &mutex_s);
                }
                threads_count++;
                current_thread++;
                usleep(sleep_time * 100000);
            }
        }
        pthread_mutex_unlock(&mutex_s);
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
    pthread_t threads[num_threads];
    int thread_ids[num_threads];
    pthread_mutex_init(&mutex_s, NULL);
    pthread_cond_init(&cond, NULL);

    first_seq_threads_done = false;
    for (int i = 0; i < num_threads; i++)
    {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, thread_function, (void *)&thread_ids[i]);
    }
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }
}
int main(int argc, char *argv[])
{

    if (argc != 6)
    {
        std::cout << "Usage: " << argv[0] << " <num_threads> <num_seq_threads> <sleep_time>" << std::endl;
        exit(1);
    }
    // watting for starting signal from parent
    wait_for_starting_signal();
    // get the data from the command line
    num_threads = atoi(argv[1]);
    num_seq_threads = atoi(argv[2]);
    sleep_time = atoi(argv[3]);
    type = argv[4];
    mq_id = atoi(argv[5]);

    // start the threads
    create_threads();
    return 0;
}