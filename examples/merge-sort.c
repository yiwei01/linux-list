#include <assert.h>
#include <stdlib.h>
#include "../include/list.h"

#include "../private/common.h"

static uint16_t values[256];

static void list_merge(struct list_head *list_left,
                       struct list_head *list_right,
                       struct list_head *head)
{
    while (!list_empty(list_left) && !list_empty(list_right)) {
        struct listitem *l_item =
            list_entry(list_left->next, struct listitem, list);
        struct listitem *r_item =
            list_entry(list_right->next, struct listitem, list);

        if (cmpint(&l_item->i, &r_item->i) < 0) {
            list_move_tail(&l_item->list, head);
        } else {
            list_move_tail(&r_item->list, head);
        }
    }

    if (!list_empty(list_left)) {
        list_splice_tail(list_left, head);
    }

    if (!list_empty(list_right)) {
        list_splice_tail(list_right, head);
    }
}

static void list_mergesort(struct list_head *head)
{
    if (list_empty(head) || list_is_singular(head))
        return;

    struct list_head list_left, list_right;

    INIT_LIST_HEAD(&list_left);
    INIT_LIST_HEAD(&list_right);

    short add_to_left = 1;
    while (!list_empty(head)) {
        if (add_to_left) {
            list_move_tail(head->next, &list_left);
        } else {
            list_move(head->prev, &list_right);
        }
        add_to_left = !add_to_left;
    }

    list_mergesort(&list_left);
    list_mergesort(&list_right);
    list_merge(&list_left, &list_right, head);
}

int main(void)
{
    struct list_head testlist;
    struct listitem *item, *is = NULL;
    size_t i;

    random_shuffle_array(values, (uint16_t) ARRAY_SIZE(values));

    INIT_LIST_HEAD(&testlist);

    assert(list_empty(&testlist));

    for (i = 0; i < ARRAY_SIZE(values); i++) {
        item = (struct listitem *) malloc(sizeof(*item));
        assert(item);
        item->i = values[i];
        list_add_tail(&item->list, &testlist);
    }

    assert(!list_empty(&testlist));

    qsort(values, ARRAY_SIZE(values), sizeof(values[0]), cmpint);
    list_mergesort(&testlist);

    i = 0;
    list_for_each_entry_safe (item, is, &testlist, list) {
        assert(item->i == values[i]);
        list_del(&item->list);
        free(item);
        i++;
    }

    assert(i == ARRAY_SIZE(values));
    assert(list_empty(&testlist));

    return 0;
}
