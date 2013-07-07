#include <stdlib.h>
#include "disposable_bag.h"

typedef struct disoposable_bag_list_item* itemptr;

void disposable_bag_init(struct disposable_bag* b){
    b->start.prev = NULL;
    b->end.next = NULL;
    b->start.next = &(b->end);
    b->end.prev= &(b->start);
}
void disposable_bag_destroy(struct disposable_bag* b){
    itemptr e,i,temp;
    e = &(b->end);
    i = b->start.next;
    while(i!=e){
        temp=i->next;
        free(i);
        i=temp;
    }
}

struct disoposable_bag_list_item* disposable_bag_add(struct disposable_bag* b,void* ptr){
    itemptr ele = malloc(sizeof(struct disoposable_bag_list_item));
    ele->data=ptr;
    ele->next=&(b->end);
    ele->prev=b->end.prev;
    ele->prev->next=ele;
    b->end.prev=ele;
    return ele;
}
void disposable_bag_dispose(struct disoposable_bag_list_item* i){
    i->prev->next=i->next;
    i->next->prev=i->prev;
}
