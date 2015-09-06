#ifndef BI_TREE_H_INCLUDED
#define BI_TREE_H_INCLUDED
#include <stdio.h>
#define MAX_USERNAME 20


struct ex_info
{
    int user_sockfd;
    char user_name[MAX_USERNAME];
};

/**
The structure of the binary tree is like this.
IP addresses are used to compare.
         4
        / \
       3   7
          / \
         6   8
*/
struct user_states
{
    unsigned long user_ip_addr;
    //struct ex_info user_ex_info;
    int user_sockfd;
    char user_name[MAX_USERNAME];
    struct user_states *lchild;
    struct user_states *rchild;
    struct user_states *parent;
};


#endif // BI_TREE_H_INCLUDED
