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

#include "bi_tree.h"
#include "connect.h"

/**
    Handle a client.
*/
void *client_session(void *arg)
{
    // get client socket file descriptor
    struct thread_args *client_info = (struct thread_args *)arg;
    struct sockaddr_in client_sockaddr;
    int client_sockaddr_size = sizeof client_sockaddr;

    //get client address info struct
    getpeername(client_info ->client_sockfd, &client_sockaddr, &client_sockaddr_size);

    // get client ip address
    char str_ip[15] = {0};
    // sin_addr  is unsigned long
    //unsigned long i = (unsigned long)((struct sockaddr_in *) &client_info -> client_sockaddr) -> sin_addr;
    unsigned long ip = (unsigned long)client_sockaddr.sin_addr.s_addr;
    inet_ntop(AF_INET,
            (void *) &ip,
            str_ip,
            sizeof str_ip);
    printf("%s\n", str_ip);

    close(client_info -> client_sockfd);
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

    return send(obj_sockfd, (void *)msg, sizeof(msg), 0);
}

int recv_pkt(int src_sockfd, struct msg_pkt* msg)
{
    if(msg == NULL)
    {
        return -1;
    }

    fflush(src_sockfd);
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
    int thread_id;

    struct thread_args args = {client_sockfd};
    int pthr_error = pthread_create(&thread_id, NULL, client_session, (void *)&args);
    if(pthr_error != 0)
    {
        perror("thread create");
        return -1;
    }
}
