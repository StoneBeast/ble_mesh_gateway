#include "link_list.h"
#include <stdlib.h>

typedef struct link_node
{
    short index;
    void *vendor_data;
    struct link_node *next;
} link_list;

static short link_list_add(link_list_handle_t list, void *node)
{
    link_list *temp_node = (link_list *)malloc(sizeof(struct link_node));
    link_list *link_p = (link_list *)list;

    if (link_p->next == NULL)
    {
        temp_node->index = 0;
    }
    else
    {
        temp_node->index = (++(link_p->next->index));
    }

    temp_node->vendor_data = node;
    temp_node->next = link_p->next;
    link_p->next = temp_node;

    return temp_node->index;
}

static void *link_list_find(link_list_handle_t list, short index)
{
    link_list *list_p = (link_list *)list;

    if (list_p->next->index < index)
    {
        return NULL;
    }
    else
    {
        link_list *node_p = list_p->next;
        for (short int i = list_p->next->index; i >= 0; i--)
        {
            if (i == index)
            {
                return node_p->vendor_data;
            }
            node_p = node_p->next;
        }

        return NULL;
    }
}

link_list_manager *link_list_manager_get(void)
{
    link_list_manager *manager = (link_list_manager *)malloc(sizeof(link_list_manager));

    link_list *list = (link_list *)malloc(sizeof(link_list));
    list->next = NULL;
    list->vendor_data = NULL;

    manager->list = (link_list_handle_t *)list;

    manager->add2list = link_list_add;
    manager->find_in_list = link_list_find;

    return manager;
}
