#include <iostream>
#include <semaphore.h>
#include "header.h"
#include "message_queues.h"
#include "shared_memory.h"
#include "shared_data.h"
// this process to ensure that the openGL will work correctly
int main()
{
    // just like simulate that this results will shown in openGL instead of there
    while (true)
    {
        sleep(1);
        cout << "employee 1 current value : " << read_shared_memory(SHARED_MEMORY_OPERATION(EMPLOYEE1)) << "\n";
        cout << "employee 2 current value : " << read_shared_memory(SHARED_MEMORY_OPERATION(EMPLOYEE2)) << "\n";
        cout << "storage area current value : " << read_shared_memory(SHARED_MEMORY_OPERATION(STORAGE_AREA)) << "\n";
        cout << "typeA_pices current value : " << read_shared_memory(SHARED_MEMORY_OPERATION(TYPEA_PICES)) << "\n";
        cout << "typeB_pices current value : " << read_shared_memory(SHARED_MEMORY_OPERATION(TYPEB_PICES)) << "\n";
        cout << "typeC_pices current value : " << read_shared_memory(SHARED_MEMORY_OPERATION(TYPEC_PICES)) << "\n";
        cout << "typeA_cartons current value : " << read_shared_memory(SHARED_MEMORY_OPERATION(CARTON_TYPEA)) << "\n";
        cout << "typeB_cartons current value : " << read_shared_memory(SHARED_MEMORY_OPERATION(CARTON_TYPEB)) << "\n";
        cout << "typeC_cartons current value : " << read_shared_memory(SHARED_MEMORY_OPERATION(CARTON_TYPEC)) << "\n";
        cout << "truckA current value : " << read_shared_memory(SHARED_MEMORY_OPERATION(TRUCKA)) << "\n";
        cout << "truckB current value : " << read_shared_memory(SHARED_MEMORY_OPERATION(TRUCKB)) << "\n";
        cout << "truckC current value : " << read_shared_memory(SHARED_MEMORY_OPERATION(TRUCKC)) << "\n";
    }
    return 0;
}
