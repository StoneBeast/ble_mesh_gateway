#ifndef _LINK_LIST_H_
#define _LINK_LIST_H_

typedef void* link_list_handle_t;

typedef struct
{
    link_list_handle_t list;
    short (*add2list)(link_list_handle_t list,
                      void *node,
                      unsigned short int node_size,
                      void *id,
                      unsigned short int id_len);
    void *(*find_by_index)(link_list_handle_t list, short index);
    void *(*find_by_id)(link_list_handle_t list, void *id);
    void  (*delete_by_id)(link_list_handle_t list, void *id);
} link_list_manager;

link_list_manager *link_list_manager_get(void);

#endif
