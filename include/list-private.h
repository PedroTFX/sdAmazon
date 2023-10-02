#ifndef _LIST_PRIVATE_H
#define _LIST_PRIVATE_H 

#include <entry.h>

struct list_t {
    struct list_t *next; //make it a linked list (this is might be hell to deal with leaks)
    struct entry_t entry;
}

#endif