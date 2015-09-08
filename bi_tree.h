#ifndef BI_TREE_H_INCLUDED
#define BI_TREE_H_INCLUDED
#include <stdio.h>
#include <stdbool.h>
#define MAX_USERNAME 20

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


struct user_info
{
    unsigned long user_ip_addr;
    char user_name[MAX_USERNAME];
    int user_sockfd;
};

void copy_node(struct user_states*, struct user_states*);
void trans_node(struct user_info*, struct user_states*);
void release_user_tree(struct user_states**);
struct user_states* create_user_tree();
bool is_leaf(struct user_states*);
void add_user(struct user_states**, struct user_states*);
struct user_states* find_user(struct user_states*, unsigned long);
struct user_states* find_max_user(struct user_states*);
struct user_states* find_min_user(struct user_states*);
int remove_user(struct user_states**, unsigned long);
int trav_tree(struct user_states*, struct user_info*);
struct user_states* create_user_node(int, unsigned long, char*);

#endif // BI_TREE_H_INCLUDED
