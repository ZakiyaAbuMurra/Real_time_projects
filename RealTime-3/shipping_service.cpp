#include "shared_memory.h"
#include "shared_data.h"
#include "message_queues.h"
#include "header.h"
int storageArea_trucks_mq_id;
sigset_t signal_set; // set of signals
pthread_mutex_t truck_mutex, truckA_count_mutex, truckB_count_mutex, truckC_count_mutex;
pthread_cond_t truck_cond;
int number_of_shipped_trucks = 0;
int truckA_count = 0, truckB_count = 0, truckC_count = 0;
// employees hows loaded the trucks
void *loading_employee(void *arg)
{

    int threadNum = *(int *)arg;
    while (true)
    {
        // stop loading the ttrucks when 3 trucks are shipped
        if (number_of_shipped_trucks == 3)
        {
            continue;
        }
        // if there is a truck avilable then load it
        if (check_mq_size(storageArea_trucks_mq_id, 0))
        {
            usleep(2000);
            if (check_mq_content(storageArea_trucks_mq_id, 1L) && truckA_count < LOADED_TYPE_A_TRUCK)
            {
                int recived_messsage = receive_Lines_mq(storageArea_trucks_mq_id, 1L);
                pthread_mutex_lock(&truckA_count_mutex);
                truckA_count += 5;
                edit_shared_memory(SHARED_MEMORY_OPERATION(TRUCKA), truckA_count);
                pthread_mutex_unlock(&truckA_count_mutex);
            }
            if (check_mq_content(storageArea_trucks_mq_id, 2L) && truckB_count < LOADED_TYPE_B_TRUCK)
            {
                int recived_messsage = receive_Lines_mq(storageArea_trucks_mq_id, 2L);
                pthread_mutex_lock(&truckB_count_mutex);
                truckB_count += 5;
                edit_shared_memory(SHARED_MEMORY_OPERATION(TRUCKB), truckB_count);
                pthread_mutex_unlock(&truckB_count_mutex);
            }
            if (check_mq_content(storageArea_trucks_mq_id, 3L) && truckC_count < LOADED_TYPE_C_TRUCK)
            {
                int recived_messsage = receive_Lines_mq(storageArea_trucks_mq_id, 3L);
                pthread_mutex_lock(&truckC_count_mutex);
                truckC_count += 5;
                edit_shared_memory(SHARED_MEMORY_OPERATION(TRUCKC), truckC_count);
                pthread_mutex_unlock(&truckC_count_mutex);
            }

            int sum = truckA_count + truckB_count + truckC_count;
            if (sum >= LOADED_TYPE_A_TRUCK + LOADED_TYPE_B_TRUCK + LOADED_TYPE_C_TRUCK)
            {
                pthread_cond_signal(&truck_cond);
            }
        }
    }
    return NULL;
}
// Thread function for type 2 threads
void *truck(void *arg)
{
    int threadNum = *(int *)arg;
    while (true)
    {
        if (threadNum == 1)
            edit_shared_memory(SHARED_MEMORY_OPERATION(IS_TRUCK1_SHIPPED), 0);
        else if (threadNum == 2)
            edit_shared_memory(SHARED_MEMORY_OPERATION(IS_TRUCK2_SHIPPED), 0);
        else if (threadNum == 3)
            edit_shared_memory(SHARED_MEMORY_OPERATION(IS_TRUCK3_SHIPPED), 0);

        // if there a truck in shipping and then come back decrement the number of shipped trucks
        if (number_of_shipped_trucks > 0)
        {
            number_of_shipped_trucks--;
        }
        pthread_mutex_lock(&truck_mutex);
        int sum;
        while (sum < LOADED_TYPE_A_TRUCK + LOADED_TYPE_B_TRUCK + LOADED_TYPE_C_TRUCK)
        {
            sum = truckA_count + truckB_count + truckC_count;
            pthread_cond_wait(&truck_cond, &truck_mutex);
        }
        pthread_mutex_lock(&truckA_count_mutex);
        truckA_count = 0;
        pthread_mutex_unlock(&truckA_count_mutex);
        pthread_mutex_lock(&truckB_count_mutex);
        truckB_count = 0;
        pthread_mutex_unlock(&truckB_count_mutex);
        pthread_mutex_lock(&truckC_count_mutex);
        truckC_count = 0;
        pthread_mutex_unlock(&truckC_count_mutex);
        pthread_mutex_unlock(&truck_mutex);

        number_of_shipped_trucks++;
        if (threadNum == 1)
            edit_shared_memory(SHARED_MEMORY_OPERATION(IS_TRUCK1_SHIPPED), 1);
        else if (threadNum == 2)
            edit_shared_memory(SHARED_MEMORY_OPERATION(IS_TRUCK2_SHIPPED), 1);
        else if (threadNum == 3)
            edit_shared_memory(SHARED_MEMORY_OPERATION(IS_TRUCK3_SHIPPED), 1);
        sleep(SHIPPING_TIME);
    }

    return NULL;
}
// create the threads for trucs 3 threads and for employyes 2 threads
void create_threads()
{
    pthread_mutex_init(&truck_mutex, NULL);
    pthread_mutex_init(&truckA_count_mutex, NULL);
    pthread_mutex_init(&truckB_count_mutex, NULL);
    pthread_mutex_init(&truckC_count_mutex, NULL);
    pthread_t type1Threads[2];
    for (int i = 0; i < 2; i++)
    {
        int *threadNum = new int(i + 1);
        pthread_create(&type1Threads[i], NULL, loading_employee, (void *)threadNum);
    }

    // Create type 2 threads
    pthread_t type2Threads[3];
    for (int i = 0; i < 3; i++)
    {
        int *threadNum = new int(i + 1);
        pthread_create(&type2Threads[i], NULL, truck, (void *)threadNum);
    }

    // Wait for all threads to complete
    for (int i = 0; i < 2; i++)
    {
        pthread_join(type1Threads[i], NULL);
    }
    for (int i = 0; i < 3; i++)
    {
        pthread_join(type2Threads[i], NULL);
    }
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
int main(int argc, char *argv[])
{
    // check if the number of arguments is correct
    if (argc != 2)
    {
        cout << "Usage: ./shipping_service <storageArea_trucks_mq_id>\n";
        exit(1);
    }
    storageArea_trucks_mq_id = atoi(argv[1]);
    wait_for_starting_signal();
    create_threads();
    return 0;
}