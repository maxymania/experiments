#ifndef DISPOSABLE_BAG_H_INCLUDED
#define DISPOSABLE_BAG_H_INCLUDED

struct disoposable_bag_list_item;
struct disoposable_bag_list_item {
    struct disoposable_bag_list_item* prev;
    struct disoposable_bag_list_item* next;
    void* data;
};
struct disposable_bag {
    struct disoposable_bag_list_item start;
    struct disoposable_bag_list_item end;
};

void disposable_bag_init(struct disposable_bag* b);
void disposable_bag_destroy(struct disposable_bag* b);

struct disoposable_bag_list_item* disposable_bag_add(struct disposable_bag* b,void* ptr);
void disposable_bag_dispose(struct disoposable_bag_list_item* i);

typedef struct disposable_bag DISPOSABLE_BAG;
typedef struct disoposable_bag_list_item* DISPOSE_ID;

#endif // DISPOSABLE_BAG_H_INCLUDED
