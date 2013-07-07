#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

struct queue_item {
    struct queue_item* next;
    void* data;
};

struct queue_list{
    struct queue_item* head;
    struct queue_item* tail;
};

struct queue{
    struct queue_list unused;
    struct queue_list used;
};

struct queue_item* queue_list_pop(struct queue_list* ql);
struct queue_item* queue_list_peek(struct queue_list* ql);
void queue_list_push(struct queue_list* ql,struct queue_item* ele);
void queue_list_destroy(struct queue_list* ql);

void queue_init(struct queue* q);

void queue_push(struct queue* q,void* obj);
void* queue_pop(struct queue* q);
void* queue_peek(struct queue* q);

void queue_destroy(struct queue* q);

typedef struct queue QUEUE;

#endif // QUEUE_H_INCLUDED
