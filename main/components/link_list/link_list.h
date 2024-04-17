#ifndef _LINK_LIST_H_
#define _LINK_LIST_H_

void link_list_init(void * list, unsigned short sizeof_node);
void link_list_add(void *list, void *node);
void link_list_find(void *list, unsigned short index);

#endif
