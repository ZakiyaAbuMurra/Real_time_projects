#ifndef SHARED_DATA
#define SHARED_DATA

#include "header.h"
using namespace std;

struct chocolate_buffer
{
    long message_type;
    int chocolate_type;
};

struct line_8_storage_area
{
    long type;
    int type_a;
    int type_b;
    int type_c;
};

struct drawer_shared_memory
{
    int employee1;
    int employee2;
    int storage_area;
    int carton_typeA;
    int carton_typeB;
    int carton_typeC;
    int typeA_pices;
    int typeB_pices;
    int typeC_pices;
    int truckA;
    int truckB;
    int truckC;
    int stop_production_lines;
    int stop_simulation;
    int is_truck1_shipped;
    int is_truck2_shipped;
    int is_truck3_shipped;
    int is_printing_expired_date;
};
enum SHARED_MEMORY_OPERATION
{
    EMPLOYEE1,
    EMPLOYEE2,
    STORAGE_AREA,
    TRUCKA,
    TRUCKB,
    TRUCKC,
    STOP_PRODUCTION_LINES,
    STOP_SIMULATION,
    CARTON_TYPEA,
    CARTON_TYPEB,
    CARTON_TYPEC,
    TYPEA_PICES,
    TYPEB_PICES,
    TYPEC_PICES,
    IS_TRUCK1_SHIPPED,
    IS_TRUCK2_SHIPPED,
    IS_TRUCK3_SHIPPED,
    IS_PRINTING_EXPIRED_DATE
};
const char *SHARED_MEMORY_SEMAPHORE = "sharedmemorysemaphore0x12345";
#define SHARED_MEMORY_KEY 0x15932
#define LINES_MQ_key 0x123456
#define LINE_8_MQ_KEY 0x24681
#define STORGAE_AREA_SHIPPING_MQ 0x13579

#define TIME_A_MIN 1
#define TIME_A_MAX 3
#define TIME_B_MIN 2
#define TIME_B_MAX 4
#define TIME_C_MIN 3
#define TIME_C_MAX 5
#define STORAGE_AREA_MIN_CAPACITY 100
#define STORAGE_AREA_MAX_CAPACITY 500
#define STORAGE_AREA_EMPLOYEE_TIME 2
#define PRINT_EXPIRED_DATE_TIME 1
#define PROGRAM_SIMULATION_TIME 300
#define TYPE_A_THRESHOLD 100
#define TYPE_B_THRESHOLD 200
#define TYPE_C_THRESHOLD 300
#define SHIPPING_TIME 30
#define LOADED_TYPE_A_TRUCK 50
#define LOADED_TYPE_B_TRUCK 100
#define LOADED_TYPE_C_TRUCK 150
#define CARTON_FILLING_TIME 3
#endif