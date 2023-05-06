#ifndef SHARED_MEMORY
#define SHARED_MEMORY
using namespace std;
#include "header.h"
#include "shared_data.h"

void create_shared_memory()
{
    int sem_id = shmget(SHARED_MEMORY_KEY, sizeof(struct drawer_shared_memory), IPC_CREAT | 0777);
    if (sem_id == -1)
    {
        perror("SEM Creation Error\n");
        exit(-1);
    }

    // For access the SEM
    struct drawer_shared_memory *smat;
    smat = (struct drawer_shared_memory *)shmat(sem_id, NULL, 0);

    if (smat == (void *)-1)
    {
        perror("SEM Attached Error\n");
        exit(-1);
    }

    // For initialize the first value
    smat->employee1 = 0;
    smat->employee2 = 0;
    smat->stop_production_lines = 0;
    smat->storage_area = 0;
    smat->truckA = 0;
    smat->truckB = 0;
    smat->truckC = 0;
    smat->stop_simulation = 0;
    smat->carton_typeA = 0;
    smat->carton_typeB = 0;
    smat->carton_typeC = 0;
    smat->typeA_pices = 0;
    smat->typeB_pices = 0;
    smat->typeC_pices = 0;
    smat->is_truck1_shipped = 0;
    smat->is_truck2_shipped = 0;
    smat->is_truck3_shipped = 0;
    smat->is_printing_expired_date = 0;

    if (shmdt(smat) == -1)
    {
        perror("SEM Detached Error\n");
        exit(-1);
    }
}

void create_SM_semaphore()
{
    sem_t *sm_sem = sem_open(SHARED_MEMORY_SEMAPHORE, O_CREAT, 0777, 0);
    if (sm_sem == SEM_FAILED)
    {
        cout << "SEM Semaphore Creation Error : " << errno << endl;
        exit(-1);
    }

    // When return -1 then wrong issue happened
    if (sem_post(sm_sem) < 0)
    {
        perror("SEM Semaphore Release Error\n");
        exit(-1);
    }
    sem_close(sm_sem);
}
void clean_SM_semaphore()
{
    if (sem_unlink(SHARED_MEMORY_SEMAPHORE) < 0)
    {
        perror("SEM Unlink Error\n");
        exit(-1);
    }
}

void edit_shared_memory(enum SHARED_MEMORY_OPERATION operation, int value)
{
    // For get the SEM
    int sm_id = shmget(SHARED_MEMORY_KEY, sizeof(struct drawer_shared_memory), IPC_CREAT | 0777);
    if (sm_id == -1)
    {
        perror("SEM Get Error\n");
        exit(-1);
    }

    // attach shared memory
    struct drawer_shared_memory *sm_mem = (struct drawer_shared_memory *)shmat(sm_id, NULL, 0);

    if (sm_mem == (void *)-1)
    {
        perror("SEM Attached Error\n");
        exit(-1);
    }

    // For open the SEM Semaphore
    sem_t *sm_sem = sem_open(SHARED_MEMORY_SEMAPHORE, O_CREAT, 0777, 0);
    if (sm_sem == SEM_FAILED)
    {
        perror("SEM Semaphore Open Error\n");
        exit(-1);
    }

    // When return 0 for successful access
    if (sem_wait(sm_sem) < 0)
    {
        perror("SEM Semaphore Wait Error\n");
        exit(-1);
    }

    // Critical Section
    // Edit the shared memory
    switch (operation)
    {
    case EMPLOYEE1:
        sm_mem->employee1 = value;
        break;
    case EMPLOYEE2:
        sm_mem->employee2 = value;
        break;
    case STOP_PRODUCTION_LINES:
        sm_mem->stop_production_lines = 1;
        break;
    case STORAGE_AREA:
        sm_mem->storage_area = value;
        break;
    case TRUCKA:
        sm_mem->truckA = value;
        break;
    case TRUCKB:
        sm_mem->truckB = value;
        break;
    case TRUCKC:
        sm_mem->truckC = value;
        break;
    case STOP_SIMULATION:
        sm_mem->stop_simulation = 1;
        break;
    case CARTON_TYPEA:
        sm_mem->carton_typeA = value;
        break;
    case CARTON_TYPEB:
        sm_mem->carton_typeB = value;
        break;
    case CARTON_TYPEC:
        sm_mem->carton_typeC = value;
        break;
    case TYPEA_PICES:
        sm_mem->typeA_pices = value;
        break;
    case TYPEB_PICES:
        sm_mem->typeB_pices = value;
        break;
    case TYPEC_PICES:
        sm_mem->typeC_pices = value;
        break;
    case IS_TRUCK1_SHIPPED:
        sm_mem->is_truck1_shipped = value;
        break;
    case IS_TRUCK2_SHIPPED:
        sm_mem->is_truck2_shipped = value;
        break;
    case IS_TRUCK3_SHIPPED:
        sm_mem->is_truck3_shipped = value;
        break;
    case IS_PRINTING_EXPIRED_DATE:
        sm_mem->is_printing_expired_date = value;
        break;
        }
    // end of critical section
    // When return -1 then wrong issue happened
    if (sem_post(sm_sem) < 0)
    {
        perror("SEM Semaphore Release Error\n");
        exit(-1);
    }

    // Close the SEM Semaphore
    sem_close(sm_sem);

    // Detach the SEM
    if (shmdt(sm_mem) == -1)
    {
        perror("SEM Detached Error\n");
        exit(-1);
    }
}

int read_shared_memory(enum SHARED_MEMORY_OPERATION operation)
{

    int result = 0;

    // For get the SEM
    int sm_id = shmget(SHARED_MEMORY_KEY, sizeof(struct drawer_shared_memory), IPC_CREAT | 0777);
    if (sm_id == -1)
    {
        perror("SEM Get Error\n");
        exit(-1);
    }
    struct drawer_shared_memory *sm_mem = (struct drawer_shared_memory *)shmat(sm_id, NULL, 0);
    // For access the SEM

    if (sm_mem == (void *)-1)
    {
        perror("SEM Attached Error\n");
        exit(-1);
    }

    // For open the SEM Semaphore
    sem_t *sm_sem = sem_open(SHARED_MEMORY_SEMAPHORE, O_CREAT, 0777, 0);
    if (sm_sem == SEM_FAILED)
    {
        perror("SEM Semaphore Open Error\n");
        exit(-1);
    }

    // When return 0 for successful access
    if (sem_wait(sm_sem) < 0)
    {
        perror("SEM Semaphore Wait Error\n");
        exit(-1);
    }

    // Critical Section
    // Read the shared memory
    switch (operation)
    {
    case EMPLOYEE1:
        result = sm_mem->employee1;
        break;
    case EMPLOYEE2:
        result = sm_mem->employee2;
        break;
    case STOP_PRODUCTION_LINES:
        result = sm_mem->stop_production_lines;
        break;
    case STORAGE_AREA:
        result = sm_mem->storage_area;
        break;
    case TRUCKA:
        result = sm_mem->truckA;
        break;
    case TRUCKB:
        result = sm_mem->truckB;
        break;
    case TRUCKC:
        result = sm_mem->truckC;
        break;
    case STOP_SIMULATION:
        result = sm_mem->stop_simulation;
        break;
    case CARTON_TYPEA:
        result = sm_mem->carton_typeA;
        break;
    case CARTON_TYPEB:
        result = sm_mem->carton_typeB;
        break;
    case CARTON_TYPEC:
        result = sm_mem->carton_typeC;
        break;
    case TYPEA_PICES:
        result = sm_mem->typeA_pices;
        break;
    case TYPEB_PICES:
        result = sm_mem->typeB_pices;
        break;
    case TYPEC_PICES:
        result = sm_mem->typeC_pices;
        break;
    case IS_TRUCK1_SHIPPED:
        result = sm_mem->is_truck1_shipped;
        break;
    case IS_TRUCK2_SHIPPED:
        result = sm_mem->is_truck2_shipped;
        break;
    case IS_TRUCK3_SHIPPED:
        result = sm_mem->is_truck3_shipped;
        break;
    case IS_PRINTING_EXPIRED_DATE:
        result = sm_mem->is_printing_expired_date;
        break;
    }
    // end of critical section

    // When return -1 then wrong issue happened
    if (sem_post(sm_sem) < 0)
    {
        perror("SEM Semaphore Release Error\n");
        exit(-1);
    }

    // Close the SEM Semaphore
    sem_close(sm_sem);

    // Detach the SEM
    if (shmdt(sm_mem) == -1)
    {
        perror("SEM Detached Error\n");
        exit(-1);
    }

    return result;
}
void clean_shared_memory()
{

    // For get the SEM
    int sem_id = shmget(SHARED_MEMORY_KEY, sizeof(struct drawer_shared_memory), IPC_CREAT | 0777);
    if (sem_id == -1)
    {
        perror("SEM Get Error\n");
        exit(-1);
    }

    // clean the memory
    if (shmctl(sem_id, IPC_RMID, 0) == -1)
    {
        perror("SEM Remove Error\n");
        exit(-1);
    }
}
#endif