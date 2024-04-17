#ifndef _LINK_LIST_H_
#define _LINK_LIST_H_

typedef void* link_list_handle_t;

typedef struct
{
    link_list_handle_t list;
    short (*add2list)(link_list_handle_t list, void *node);
    void *(*find_in_list)(link_list_handle_t list, short index);
} link_list_manager;

link_list_manager *link_list_manager_get(void);

#endif
