#include <stdlib.h>
#include "queue.h"

struct queue_item* queue_list_pop(struct queue_list* ql){
    struct queue_item* res = ql->head;
    if(ql->head!=ql->tail)ql->head=ql->head->next;
    else ql->head=ql->tail=NULL;
    return res;
}
struct queue_item* queue_list_peek(struct queue_list* ql){
    return ql->head;
}
void queue_list_push(struct queue_list* ql,struct queue_item* ele){
    if(ql->head&&ql->tail){
        ql->tail->next = ele;
        ql->tail = ele;
    }else{
        ql->head = ql->tail=ele;
    }
}
void queue_list_destroy(struct queue_list* ql){
    struct queue_item* res = ql->head;
    struct queue_item* temp;
    while(res){
        temp=res->next;
        free(res);
        res=temp;
    }
}

void queue_init(struct queue* q){
    *q = (struct queue){{NULL,NULL},{NULL,NULL}};
}

void queue_push(struct queue* q,void* obj){
    struct queue_item* item = queue_list_pop(&q->unused);
    if(!item) item = malloc(sizeof(struct queue_item));
    *item=(struct queue_item){NULL,obj};
    queue_list_push(&q->used,item);
}
void* queue_pop(struct queue* q){
    void* res=NULL;
    struct queue_item* item = queue_list_pop(&q->used);
    if(item){
        item->next=NULL;
        res=item->data;
        queue_list_push(&q->unused,item);
    }
    return res;
}
void* queue_peek(struct queue* q){
    void* res=NULL;
    struct queue_item* item = queue_list_peek(&q->used);
    if(item){
        res=item->data;
    }
    return res;
}

void queue_destroy(struct queue* q){
    queue_list_destroy(&q->unused);
    queue_list_destroy(&q->used);
}


