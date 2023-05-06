#include "header.h"
#include "shared_data.h"
#include "message_queues.h"
#include "shared_memory.h"
using namespace std;
/*----------------------------------------------------------------------------------------------------*/
// functions and variables for parent
vector<int> lines_pids;
int line_8_process_pid;
pid_t storage_area_pid;
pid_t shipping_pid;
pid_t create_process(const char *name, int number_of_workers, int number_of_inorder_workers, int time, const char *type);
int read_file();
void create_line_8();
void create_lines();
int lines_mq_id;
int line_8_sorageArea_mq_id;
int storageArea_trucks_mq_id;
void start_process();
void kill_childs_and_delete_ipcs();
int random_number(int min, int max);
void create_ipcs();
void create_storageArea();
void create_shippingService();
void wait_to_finish_simulation();
int drawer_pid;
/*----------------------------------------------------------------------------------------------------*/

int main()
{

    // create data structures
    create_ipcs();
    // create production lines
    create_lines();
    // create line 8
    create_line_8();
    // create storage area
    create_storageArea();
    // create shipping service
    create_shippingService();
    sleep(2);
    drawer_pid = create_process("./drawer", 0, 0, 0, "");
    // sleep for 2 seconds to ensure all processes are created properly
    sleep(2);
    // start simulation
    start_process();

    // wait for simulation to finish
    wait_to_finish_simulation();

    // kill all processes and delete ipcs after simulation is finished
    kill_childs_and_delete_ipcs();
    return 0;
}

void wait_to_finish_simulation()
{
    int time_counter = 0;

    while (true)
    {
        sleep(1);
        time_counter++;
        if (read_shared_memory(SHARED_MEMORY_OPERATION(STOP_SIMULATION)) == 1)
        {
            cout << "Simulation stopped due the quantity of each type" << endl;
            break;
        }
        if (time_counter == PROGRAM_SIMULATION_TIME)
        {
            cout << "Simulation stopped due the time" << endl;
            break;
        }
    }
}
pid_t create_process(const char *name, int number_of_workers, int number_of_inorder_workers, int time, const char *type)
{
    pid_t pid;
    pid = fork();

    if (pid == 0)
    {
        if (name == "./line")
        {
            execlp(name, name, to_string(number_of_workers).data(), to_string(number_of_inorder_workers).data(), to_string(time).data(), type, to_string(lines_mq_id).data(), (char *)NULL);
            perror("Error in exec line");
            exit(1);
        }
        if (name == "./line_8")
        {
            execlp(name, name, to_string(lines_mq_id).data(), to_string(line_8_sorageArea_mq_id).data(), (char *)NULL);
            perror("Error in exec line_8");
            exit(1);
        }
        if (name == "./storageArea")
        {
            execlp(name, name, to_string(line_8_sorageArea_mq_id).data(), to_string(storageArea_trucks_mq_id).data(), (char *)NULL);
            perror("Error in exec storageArea");
            exit(1);
        }
        if (name == "./shippingService")
        {
            execlp(name, name, to_string(storageArea_trucks_mq_id).data(), (char *)NULL);
            perror("Error in exec shppingService");
            exit(1);
        }
        if (name == "./drawer")
        {
            execlp(name, name, (char *)NULL);
            perror("Error in exec drawer");
            exit(1);
        }
    }
    else if (pid < 0)
    {
        printf("Fork failed.\n");
    }
    return pid;
}
// create lines processes
void create_lines()
{
    int time;
    for (int i = 0; i < 3; i++)
    {
        time = random_number(TIME_A_MIN, TIME_A_MAX);
        pid_t pid = create_process("./line", 8, 4, time, "A");
        lines_pids.push_back(pid);
    }
    for (int i = 0; i < 2; i++)
    {
        time = random_number(TIME_B_MIN, TIME_B_MAX);
        pid_t pid = create_process("./line", 6, 4, time, "B");
        lines_pids.push_back(pid);
    }
    for (int i = 0; i < 2; i++)
    {
        time = random_number(TIME_C_MIN, TIME_C_MAX);
        pid_t pid = create_process("./line", 5, 3, time, "C");
        lines_pids.push_back(pid);
    }
}
// kill all processes and delete ipcs
void kill_childs_and_delete_ipcs()
{
    // kill the child process which thier pids stode in the vector
    for (int i = 0; i < lines_pids.size(); i++)
    {
        kill(lines_pids[i], SIGKILL);
    }
    kill(line_8_process_pid, SIGKILL);
    kill(storage_area_pid, SIGKILL);
    kill(shipping_pid, SIGKILL);
    kill(drawer_pid, SIGKILL);
    // delete the message queues
    delete_mq(lines_mq_id);
    delete_mq(line_8_sorageArea_mq_id);
    delete_mq(storageArea_trucks_mq_id);
    // delete the shared memory and semaphore
    clean_SM_semaphore();
    clean_shared_memory();
}
// create the line 8 process
void create_line_8()
{
    line_8_process_pid = create_process("./line_8", 0, 0, 0, "");
}
// create the storage area process
void create_storageArea()
{
    storage_area_pid = create_process("./storageArea", 0, 0, 0, "");
}
// create the shipping service process
void create_shippingService()
{
    shipping_pid = create_process("./shippingService", 0, 0, 0, "");
}
// start all the process
void start_process()
{

    int status;
    for (int i = 0; i < lines_pids.size(); i++)
    {
        status = kill(lines_pids[i], SIGUSR1);
        if (status == -1)
        {
            cout << "Error sending signal to process " << lines_pids[i] << endl;
            exit(1);
        }
    }
    usleep(100000);
    status = kill(line_8_process_pid, SIGUSR1);
    if (status == -1)
    {
        cout << "Error sending signal to process " << line_8_process_pid << endl;
        exit(1);
    }
    usleep(100000);
    status = kill(storage_area_pid, SIGUSR1);
    if (status == -1)
    {
        cout << "Error sending signal to process " << storage_area_pid << endl;
        exit(1);
    }
    usleep(100000);
    status = kill(shipping_pid, SIGUSR1);
    if (status == -1)
    {
        cout << "Error sending signal to process " << shipping_pid << endl;
        exit(1);
    }
    usleep(100000);
    status = kill(drawer_pid, SIGUSR1);
}
// randomly generated number in rang
int random_number(int min, int max)
{
    srand(time(NULL) ^ (getpid() << 16));
    return rand() % (max - min + 1) + min;
}
// create the message queue and shared memory and semaphore
void create_ipcs()
{
    lines_mq_id = create_mq(LINES_MQ_key);
    line_8_sorageArea_mq_id = create_mq(LINE_8_MQ_KEY);
    storageArea_trucks_mq_id = create_mq(STORGAE_AREA_SHIPPING_MQ);
    create_shared_memory();
    create_SM_semaphore();
}