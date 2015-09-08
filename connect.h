#ifndef CONNECT_H_INCLUDED
#define CONNECT_H_INCLUDED

#include "bi_tree.h"
#include "stdio.h"
#define QUEUE 10
#define THREAD int
#define MAX_MSG 1024
#define MAX_BUFF sizeof(struct msg_pkt)
#define MAX_FR_LS 30
#define MAX_MULTI_CHAT 10

struct thread_args
{
    int client_sockfd;
};

enum FLAG{
    HELLO, // client say hello to server
    BYE,
    MSG, // client sends messages to friends
    RES_CHAT, // clients request to chat with a group of people
    FR_LS // client requires friend list
};

struct chat_msg
{
    int session_id;
    int size;
    struct user_info dest_users[MAX_MULTI_CHAT];
    char msg_str[MAX_MSG];
};

struct friends_info
{
    int size;
    struct user_info users[MAX_FR_LS];
};

/**

                            The structure of packect
┍================================================================================┑
|                                [int length]                                    |
|================================================================================|
|  [enum FLAG flag]   HELLO, BYE, MSG, RES_CHAT, FR_LS                           |
|================================================================================|
|                            [union data msg_data]                               |
|................................................................................|
|       struct chat_msg message         |      strcut friends_info friends       |
|.......................................|........................................|
|           [int session_id]            |               [int size]               |
|               [int size]              |   ----------------------------------   |
|           [char msg_str[]]            |   |  struct user_info dest_users[] |   |
|   ---------------------------------   |   |................................|   |
|   | struct user_info dest_users[] |   |   |   [unsigned long user_ip_addr] |   |
|   |...............................|   |   |       [char user_name[]]       |   |
|   |   [unsigned long user_ip_addr]|   |   |       [int user_sockfd]        |   |
|   |       [char user_name[]]      |   |   ----------------------------------   |
|   |       [int user_sockfd]       |   |                  .                     |
|   ---------------------------------   |                  .                     |
|                  .                    |                  .                     |
|                  .                    |                                        |
|                  .                    |                                        |
==================================================================================
*/
struct msg_pkt
{
    int length;
    enum FLAG flag;
    union data{
        struct chat_msg message;
        struct friends_info friends;
    } msg_data;
};

//void log(char*, char*);
void broadcast(struct msg_pkt*);
void* client_session(void*);
int start_server(int);
int send_pkt(int, struct msg_pkt*);
int recv_pkt(int, struct msg_pkt*);
int wait_for_new_clients(int);

#endif // CONNECT_H_INCLUDED
