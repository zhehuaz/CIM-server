#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include "connect.h"

extern struct user_states* users;
pthread_mutex_t mutex_users = PTHREAD_MUTEX_INITIALIZER;
int session_num = 0;


void log(char* from, char* log_content)
{
    printf("%s : %s\n", from, log_content);
}

void broadcast(struct msg_pkt* msg)
{
    struct user_info arr_users[MAX_FR_LS];

    pthread_mutex_lock(&mutex_users);
    int size = trav_tree(users, arr_users);
    pthread_mutex_unlock(&mutex_users);

    for(int i = 0;i < size;i ++)
    {
        //send_msg.msg_data.
        send_pkt(arr_users[i].user_sockfd, msg);
    }
}

/**
    Handle a client.
*/
void *client_session(void *arg)
{
    // get client socket file descriptor
    int client_sockfd = ((struct thread_args *)arg) -> client_sockfd;
    struct sockaddr_in client_sockaddr;
    int client_sockaddr_size = sizeof client_sockaddr;

    //get client address info struct
    getpeername(client_sockfd, &client_sockaddr, &client_sockaddr_size);

    // get client ip address
    char str_ip[15] = {0};

    // unsigned long i = (unsigned long)((struct sockaddr_in *) &client_info -> client_sockaddr) -> sin_addr;
    unsigned long ip = (unsigned long)client_sockaddr.sin_addr.s_addr;
    inet_ntop(AF_INET,
            (void *) &ip,
            str_ip,
            sizeof str_ip);

    log(str_ip, "Online");
    // save this user to user list
    struct user_states* client = create_user_node(client_sockfd, ip, "default");
    // ********* TODO why not create_user_node()?? *******
    //struct user_states* client = (struct user_states*) malloc(sizeof(struct user_states));
    //client ->user_sockfd = client_sockfd;
    //client ->user_ip_addr = ip;
    //strcpy(client -> user_name, "default");

    pthread_mutex_lock(&mutex_users);
    add_user(&users, client);
    pthread_mutex_unlock(&mutex_users);


    int is_on = 1;

    struct msg_pkt send_msg;
    struct msg_pkt recv_msg;

    while(1)
    {
        memset(&recv_msg, 0, sizeof recv_msg);
        memset(&send_msg, 0, sizeof send_msg);
        is_on = recv_pkt(client_sockfd, &recv_msg);

        // A user is offline. Tell all the online users about this.
        if(is_on == 0)
        {
            log(str_ip, "Offline");

            pthread_mutex_lock(&mutex_users);
            trans_node(&send_msg.msg_data.friends.users[0], client);
            remove_user(&users, ip);
            pthread_mutex_unlock(&mutex_users);

            send_msg.msg_data.friends.size = 1;
            send_msg.flag = BYE;
            broadcast(&send_msg);
            break;
        }

        // Handle this message from the client
        switch(recv_msg.flag)
        {
            /**
            *   Everytime a client connect to the server, it sends a "hello" packet
            *   to the server, in order that let the server know its new name, which
            *   is "default" by default.The server log the new username in the online
            *   users' tree, and broadcast the new-online message to let all know.
            */
            case HELLO:
                log(str_ip, "Hello!");
                if(recv_msg.msg_data.message.msg_str != NULL)
                    strcpy(client -> user_name, recv_msg.msg_data.message.msg_str);

                pthread_mutex_lock(&mutex_users);
                trans_node(&send_msg.msg_data.friends.users[0], client);
                pthread_mutex_unlock(&mutex_users);

                send_msg.msg_data.friends.size = 1;
                send_msg.flag = HELLO;
                broadcast(&send_msg);
                break;
            /**
            *   A client wants to send a message to some clients.The server is responsible
            *   to transfer this message.
            */
            case MSG:
                log(str_ip, "Hey, you");
                for(int i = 0;i < recv_msg.msg_data.message.size; i ++)
                {
                    send_pkt(recv_msg.msg_data.message.dest_users[i].user_sockfd, &recv_msg);
                }
                break;
            /**
            *   A client asks "Who's on?" to the server, and the server responds an array of
            *   all the online clients' infomation generated from the users' tree.
            */
            case FR_LS:
                log(str_ip, "Who's on");
                // ******* TODO if friends' list is too large, split the message ******
                send_msg.flag = FR_LS;
                send_msg.msg_data.friends.size = trav_tree(users, send_msg.msg_data.friends.users);
                send_msg.length = sizeof send_msg;
                send_pkt(client_sockfd,&send_msg);
                break;
            /**
            *   Before chat to a group of clients, the requesting client need to send a request
            *   the server.Then, the server tell the other clients that one client want to chat
            *   with all of them so that they get prepared.
            *   A session defines a chat group with some clients in it sending and receiving
            *   messages.The server maintains a session number to keep the session ids of all the
            *   clients consistent and unique.
            */
            case RES_CHAT:
                log(str_ip, "Let's play");

                //struct user_states *pointer;
                send_msg.msg_data.message.size = 0;
                int size = recv_msg.msg_data.message.size;
                pthread_mutex_lock(&mutex_users);
                for(int i = 0; i < size; i ++)
                {
                    struct user_states *pointer = find_user(users, recv_msg.msg_data.message.dest_users[i].user_ip_addr);
                    if(pointer != NULL)
                    {
                        trans_node(&send_msg.msg_data.message.dest_users[send_msg.msg_data.message.size ++],
                            pointer);
                    }
                    else
                    {
                        // TODO Do I need to tell him that this guy has gone?
                        log("host", "Can't find this one.");
                    }
                }
                send_msg.msg_data.message.session_id = session_num ++;
                pthread_mutex_unlock(&mutex_users);
                send_msg.flag = RES_CHAT;
                for(int i = 0;i < send_msg.msg_data.message.size;i ++)
                {
                    send_pkt(send_msg.msg_data.message.dest_users[i].user_sockfd , &send_msg);
                }
                break;
        }
    }


    close(client_sockfd);
}

int start_server(int port)
{
	//create socket
    int server_sockfd = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
	if(server_sockfd < 0)
		perror("socket creating failed");

    //Build address structure to bind to socket.
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(port);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind,return -1 when failed,return 0 when succeeded
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr)) < 0)
        perror("bind failed");

    //listen,return -1 when failed,return 0 when succeeded
    if(listen(server_sockfd,QUEUE) < 0)
        perror("listen failed");

    return server_sockfd;
}

int send_pkt(int obj_sockfd, struct msg_pkt* msg)
{
    if(msg == NULL)
    {
        return -1;
    }

    return send(obj_sockfd, (void *)msg, sizeof(*msg), 0);
}

int recv_pkt(int src_sockfd, struct msg_pkt* msg)
{
    if(msg == NULL)
    {
        return -1;
    }

    //fflush(src_sockfd);
    return recv(src_sockfd, (void *)msg, MAX_BUFF, 0);
}

int wait_for_new_clients(int server_sockfd)
{
    int client_sockfd;
    THREAD client_thread;
    struct sockaddr_in client_sockaddr;
    int client_sockaddr_size = sizeof client_sockaddr;
    client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_sockaddr, &client_sockaddr_size);
    if(client_sockfd == -1)
    {
        perror("accept");
        return -1;
    }

    struct thread_args args = {client_sockfd};
    int pthr_error = pthread_create(&client_thread, NULL, client_session, (void *)&args);
    if(pthr_error != 0)
    {
        perror("thread create");
        return -1;
    }
    return 0;
}
