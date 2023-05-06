#ifndef MESSAGE_QUEUES
#define MESSAGE_QUEUES
#include "header.h"
#include "shared_data.h"
using namespace std;

int create_mq(int mq_key)
{
    int msgid = msgget(mq_key, 0777 | IPC_CREAT);
    if (msgid == -1)
    {
        cout << "Error creating message queue" << endl;
        exit(1);
    }
    return msgid;
}

void send_lines_mq(int type, int mq_id)
{

    struct chocolate_buffer new_message;
    new_message.message_type = (long)type;

    new_message.chocolate_type = type;
    if (msgsnd(mq_id, &new_message, sizeof(chocolate_buffer), 0) == -1)
    {
        cout << "Error sending message with type : " << type << " id: " << mq_id << " error : " << errno << endl;
        exit(1);
    }
}

int receive_Lines_mq(int mq_id, long type)
{
    struct chocolate_buffer recived_message;
    if (msgrcv(mq_id, &recived_message, sizeof(struct chocolate_buffer), type, 0) == -1)
    {
        cout << "Error receiving message" << endl;
        exit(1);
    }
    // cout << "Received message: " << recived_message.chocolate_type << endl;
    return recived_message.chocolate_type;
}

void send_line8_mq(int type_A, int type_B, int type_C, int mq_id)
{

    struct line_8_storage_area new_message;
    new_message.type = 1L;

    new_message.type_a = type_A;
    new_message.type_b = type_B;
    new_message.type_c = type_C;
    if (msgsnd(mq_id, &new_message, sizeof(line_8_storage_area), 0) == -1)
    {
        cout << "Error sending message error: " << errno << endl;
        exit(1);
    }
}
struct line_8_storage_area receive_Line8_mq(int mq_id, long type)
{
    struct line_8_storage_area recived_message;
    if (msgrcv(mq_id, &recived_message, sizeof(struct line_8_storage_area), type, 0) == -1)
    {
        cout << "Error receiving message" << endl;
        exit(1);
    }

    return recived_message;
}
void delete_mq(int mq_id)
{
    int ret = 0;

    if ((ret = msgctl(mq_id, IPC_RMID, NULL)) == -1)
    {
        cout << "Could not delete message queue. " << endl;
        exit(1);
    }
}

int check_mq_size(int queue, long queue_type)
{
    // two teqniqes to check if queue is empty

    struct msqid_ds queue_info;
    msgctl(queue, IPC_STAT, &queue_info);
    if (queue_info.msg_qnum > 0)
    {
        return queue_info.msg_qnum; // There is data
    }
    return 0; // Empty queue
    /*--------------------------------------------------------------*/
    // if (msgrcv(queue, NULL, 0, queue_type, IPC_NOWAIT) == -1)
    // {
    //     if (errno == E2BIG)
    //         return 1; // There is data
    // }
    // return 0; // Empty queue
}
int check_mq_content(int queue, long queue_type)
{
    // two teqniqes to check if queue is empty

    // struct msqid_ds queue_info;
    // msgctl(queue, IPC_STAT, &queue_info);
    // if (queue_info.msg_qnum > 0)
    // {
    //     return queue_info.msg_qnum; // There is data
    // }
    // return 0; // Empty queue
    /*--------------------------------------------------------------*/
    if (msgrcv(queue, NULL, 0, queue_type, IPC_NOWAIT) == -1)
    {
        if (errno == E2BIG)
            return 1; // There is data
    }
    return 0; // Empty queue
}
#endif