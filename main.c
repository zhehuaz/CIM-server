/*
** server.c -- a stream socket server demo
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "bi_tree.h"
#include "connect.h"

const int port = 25582;

struct user_states* users;


int main()
{
 /*
    Binary tree test.

    struct user_states* root = create_user_tree();

    struct user_states* users[10];

    for(int i = 0;i < 10;i ++)
    {
        users[i] = (struct user_states*) malloc(sizeof(struct user_states));
        users[i] -> user_ip_addr = 10 + abs(3 * i - 5);
        add_user(&root, users[i]);
    }

    struct user_states users_cp[100];
    int size = trav_tree(root, users_cp);
    for(int i = 0;i < size;i ++)
    {
        printf("%d\n", users_cp[i].user_ip_addr);
    }

    release_user_tree(&root);*/

    users = create_user_tree();
    int server_sockfd = start_server(port);
    bool is_running = true;
    while(is_running)
    {
        wait_for_new_clients(server_sockfd);
    }
    release_user_tree(&users);


}
