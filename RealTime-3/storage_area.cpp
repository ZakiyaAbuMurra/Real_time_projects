#include "header.h"
#include "shared_data.h"
#include "message_queues.h"
#include "shared_memory.h"
sigset_t signal_set; // set of signals
int line_8_storageArea_mq_id;
int storageArea_shipping_mq_id;
int
    typeA_pices = 0,
    typeB_pices = 0, typeC_pices = 0, typeA_cartons = 0, typeB_cartons = 0, typeC_cartons = 0, storage_area_count = 0,
    typeA_carton_check = 0, typeB_carton_check = 0, typeC_carton_check = 0;

pthread_mutex_t typeA_pices_data_mutex;
pthread_mutex_t typeB_pices_data_mutex;
pthread_mutex_t typeC_pices_data_mutex;
pthread_mutex_t typeA_cartons_data_mutex;
pthread_mutex_t typeB_cartons_data_mutex;
pthread_mutex_t typeC_cartons_data_mutex;

pthread_cond_t typeA_pices_cond;
pthread_cond_t typeB_pices_cond;
pthread_cond_t typeC_pices_cond;
pthread_cond_t typeA_cartons_cond;
pthread_cond_t typeB_cartons_cond;
pthread_cond_t typeC_cartons_cond;

void *seperate_pices_employee(void *arg)
{
    int id = *((int *)arg);
    while (true)
    {
        if (check_mq_size(line_8_storageArea_mq_id, 0))
        {
            struct line_8_storage_area received_message = receive_Line8_mq(line_8_storageArea_mq_id, 1);
            if (received_message.type_a > 0)
            {
                pthread_mutex_lock(&typeA_pices_data_mutex);
                typeA_pices += received_message.type_a;
                edit_shared_memory(SHARED_MEMORY_OPERATION(TYPEA_PICES), typeA_pices);
                if (typeA_pices >= 20)
                    pthread_cond_signal(&typeA_pices_cond);

                pthread_mutex_unlock(&typeA_pices_data_mutex);
            }
            if (received_message.type_b > 0)
            {
                pthread_mutex_lock(&typeB_pices_data_mutex);
                typeB_pices += received_message.type_b;
                edit_shared_memory(SHARED_MEMORY_OPERATION(TYPEB_PICES), typeB_pices);
                if (typeB_pices >= 20)
                    pthread_cond_signal(&typeB_pices_cond);
                pthread_mutex_unlock(&typeB_pices_data_mutex);
            }
            if (received_message.type_c > 0)
            {
                pthread_mutex_lock(&typeC_pices_data_mutex);
                typeC_pices += received_message.type_c;
                edit_shared_memory(SHARED_MEMORY_OPERATION(TYPEC_PICES), typeC_pices);
                if (typeC_pices >= 20)
                    pthread_cond_signal(&typeC_pices_cond);
                pthread_mutex_unlock(&typeC_pices_data_mutex);
            }
        }
    }
    return NULL;
}

void *carton_filling_employee(void *arg)
{
    int id = *((int *)arg);
    while (true)
    {
        if (id == 0)
        {
            pthread_mutex_lock(&typeA_pices_data_mutex);
            while (typeA_pices < 20)
                pthread_cond_wait(&typeA_pices_cond, &typeA_pices_data_mutex);
            typeA_pices -= 20;
            pthread_mutex_lock(&typeA_cartons_data_mutex);
            typeA_cartons++;
            typeA_carton_check++;
            edit_shared_memory(SHARED_MEMORY_OPERATION(CARTON_TYPEA), typeA_cartons);
            usleep(CARTON_FILLING_TIME * 100000);
            if (typeA_cartons >= 5)
                pthread_cond_signal(&typeA_cartons_cond);
            pthread_mutex_unlock(&typeA_cartons_data_mutex);
            pthread_mutex_unlock(&typeA_pices_data_mutex);
        }
        else if (id == 1)
        {
            pthread_mutex_lock(&typeB_pices_data_mutex);
            while (typeB_pices < 20)
                pthread_cond_wait(&typeB_pices_cond, &typeB_pices_data_mutex);
            typeB_pices -= 20;
            pthread_mutex_lock(&typeB_cartons_data_mutex);
            typeB_cartons++;
            typeB_carton_check++;
            edit_shared_memory(SHARED_MEMORY_OPERATION(CARTON_TYPEB), typeB_cartons);
            usleep(CARTON_FILLING_TIME * 100000);
            if (typeB_cartons >= 5)
                pthread_cond_signal(&typeB_cartons_cond);
            pthread_mutex_unlock(&typeB_cartons_data_mutex);
            pthread_mutex_unlock(&typeB_pices_data_mutex);
        }
        else if (id == 2)
        {

            pthread_mutex_lock(&typeC_pices_data_mutex);
            while (typeC_pices < 20)
                pthread_cond_wait(&typeC_pices_cond, &typeC_pices_data_mutex);
            typeC_pices -= 20;
            pthread_mutex_lock(&typeC_cartons_data_mutex);
            typeC_cartons++;
            typeC_carton_check++;
            edit_shared_memory(SHARED_MEMORY_OPERATION(CARTON_TYPEC), typeC_cartons);
            usleep(CARTON_FILLING_TIME * 100000);
            if (typeC_cartons >= 5)
                pthread_cond_signal(&typeC_cartons_cond);
            pthread_mutex_unlock(&typeC_cartons_data_mutex);
            pthread_mutex_unlock(&typeC_pices_data_mutex);
        }
        // check if the simulation should stop
        if (typeA_carton_check > TYPE_A_THRESHOLD && typeB_carton_check > TYPE_B_THRESHOLD && typeC_carton_check > TYPE_C_THRESHOLD)
        {
            edit_shared_memory(SHARED_MEMORY_OPERATION(STOP_SIMULATION), 1);
        }
    }
    return NULL;
}

void *storage_area_employee(void *arg)
{
    int id = *((int *)arg);
    while (true)
    { // mutex the type_A_cartons
        pthread_mutex_lock(&typeA_cartons_data_mutex);
        while (typeA_cartons < 5)
            pthread_cond_wait(&typeA_cartons_cond, &typeA_cartons_data_mutex);
        typeA_cartons -= 5;
        // sleep absent time
        usleep(STORAGE_AREA_EMPLOYEE_TIME * 100000);
        // send the message to the storage area
        send_lines_mq(1, storageArea_shipping_mq_id);
        pthread_mutex_unlock(&typeA_cartons_data_mutex);

        // mutex the type_B_cartons
        pthread_mutex_lock(&typeB_cartons_data_mutex);
        while (typeB_cartons < 5)
            pthread_cond_wait(&typeB_cartons_cond, &typeB_cartons_data_mutex);
        typeB_cartons -= 5;
        // sleep absent time
        usleep(STORAGE_AREA_EMPLOYEE_TIME * 100000);
        // send the message to the storage area
        send_lines_mq(2, storageArea_shipping_mq_id);
        pthread_mutex_unlock(&typeB_cartons_data_mutex);

        // mutex the type_C_cartons
        pthread_mutex_lock(&typeC_cartons_data_mutex);
        while (typeC_cartons < 5)
            pthread_cond_wait(&typeC_cartons_cond, &typeC_cartons_data_mutex);
        typeC_cartons -= 5;
        // sleep absent time
        usleep(STORAGE_AREA_EMPLOYEE_TIME * 100000);
        // lock the storage_area_count
        // send the message to the storage area
        send_lines_mq(3, storageArea_shipping_mq_id);

        pthread_mutex_unlock(&typeC_cartons_data_mutex);
        // print the value of sorage area based on the size of the queue
        edit_shared_memory(SHARED_MEMORY_OPERATION(STORAGE_AREA), check_mq_size(storageArea_shipping_mq_id, 0) * 5);
        if (check_mq_size(storageArea_shipping_mq_id, 0) * 5 >= STORAGE_AREA_MAX_CAPACITY)
        {
            // stop the production lines
            edit_shared_memory(SHARED_MEMORY_OPERATION(STOP_PRODUCTION_LINES), 1);
        }
    }
    return NULL;
}
void init_mutex_and_cond()
{
    pthread_mutex_init(&typeA_pices_data_mutex, NULL);
    pthread_mutex_init(&typeB_pices_data_mutex, NULL);
    pthread_mutex_init(&typeC_pices_data_mutex, NULL);
    pthread_mutex_init(&typeA_cartons_data_mutex, NULL);
    pthread_mutex_init(&typeB_cartons_data_mutex, NULL);
    pthread_mutex_init(&typeC_cartons_data_mutex, NULL);
    pthread_cond_init(&typeA_pices_cond, NULL);
    pthread_cond_init(&typeB_pices_cond, NULL);
    pthread_cond_init(&typeC_pices_cond, NULL);
    pthread_cond_init(&typeA_cartons_cond, NULL);
    pthread_cond_init(&typeB_cartons_cond, NULL);
    pthread_cond_init(&typeC_cartons_cond, NULL);
}
void create_threads()
{
    init_mutex_and_cond();
    pthread_t firstTypeThreads[3];
    pthread_t secondTypeThreads[3];
    pthread_t thirdTypeThreads[2];
    // create the first two types
    for (int i = 0; i < 3; i++)
    {
        pthread_create(&firstTypeThreads[i], NULL, seperate_pices_employee, (void *)&i);
        pthread_create(&secondTypeThreads[i], NULL, carton_filling_employee, (void *)&i);
    }
    // create the third type
    for (int i = 0; i < 2; i++)
    {
        pthread_create(&thirdTypeThreads[i], NULL, storage_area_employee, (void *)&i);
    }
    // join the first two types
    for (int i = 0; i < 3; i++)
    {
        pthread_join(firstTypeThreads[i], NULL);
        pthread_join(secondTypeThreads[i], NULL);
    }
    // join the third type
    for (int i = 0; i < 2; i++)
    {
        pthread_join(thirdTypeThreads[i], NULL);
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
    if (argc != 3)
    {
        printf("Usage: ./storage_area <line_8_storageArea_mq_id> <storageArea_shipping_mq_id>}\n");
        return 1;
    }
    line_8_storageArea_mq_id = atoi(argv[1]);
    storageArea_shipping_mq_id = atoi(argv[2]);
    wait_for_starting_signal();
    create_threads();
    return 0;
}