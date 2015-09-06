#ifndef CONNECT_H_INCLUDED
#define CONNECT_H_INCLUDED

#include "bi_tree.h"

#define QUEUE 10
#define THREAD int
#define MAX_MSG 1024
#define MAX_BUFF 2046
#define MAX_FR_LS 50

struct thread_args
{
    int client_sockfd;
};

enum FLAG{
    HELLO, // clinet say hello to server
    MSG, // client sends messages to friends
    FR_LS // client requires friend list
};

struct msg_pkt
{
    int length;
    int flag;
    unsigned long des_ip;
    union data{
        char msg_str[MAX_MSG]; // the first four chars is the session id
        struct user_states users[MAX_FR_LS];
    };
};


#endif // CONNECT_H_INCLUDED
