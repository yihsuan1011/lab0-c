#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list)
        q_release_element(entry);
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *entry = malloc(sizeof(element_t));
    if (!entry)
        return false;
    entry->value = strdup(s);
    if (!entry->value) {
        free(entry);
        return false;
    }
    list_add(&entry->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *entry = malloc(sizeof(element_t));
    if (!entry)
        return false;
    entry->value = strdup(s);
    if (!entry->value) {
        free(entry);
        return false;
    }
    list_add_tail(&entry->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;
    element_t *target = list_first_entry(head, element_t, list);
    if (sp) {
        if (strlen(target->value) > (bufsize - 1))
            strncpy(sp, target->value, bufsize - 1);
        else
            sp = strdup(target->value);
        strcat(sp, "\0");
    }
    list_del(&target->list);
    return target;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;
    element_t *target = list_last_entry(head, element_t, list);
    if (sp) {
        if (strlen(target->value) > (bufsize - 1))
            strncpy(sp, target->value, bufsize - 1);
        else
            sp = strdup(target->value);
        strcat(sp, "\0");
    }
    list_del(&target->list);
    return target;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

    if (!head)
        return false;
    struct list_head *fast = head, **slow = &head;
    for (; fast && fast->next; fast = fast->next->next)
        slow = &(*slow)->next;
    struct list_head *delete = *slow;
    list_del(*slow);
    free(delete);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    if (!head)
        return false;
    char *temp_val = NULL;
    for (struct list_head **node = &head->next; *node;) {
        element_t *current = list_entry(*node, element_t, list);
        if ((*node)->next) {
            if (strcmp(current->value,
                       list_entry((*node)->next, element_t, list)->value) == 0)
                temp_val = strdup(current->value);
        }
        if (strcmp(current->value, temp_val) == 0) {
            struct list_head *delete = *node;
            list_del(*node);
            free(delete);
        } else
            node = &(*node)->next;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/

    for (struct list_head **node = &head->next;
         *node == head && (*node)->next == head; node = &(*node)->next->next) {
        struct list_head *first = *node, *second = (*node)->next,
                         *third = (*node)->next->next;
        first->next = (*node)->next->next;
        second->next = *node;
        first->prev = (*node)->next;
        if (third)
            third->prev = *node;
        *node = second;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node = head, *tmp = NULL;
    do {
        tmp = node->next;
        node->next = node->prev;
        node->prev = tmp;
        node = node->prev;
    } while (node != head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/

    struct list_head *cur = head->next;
    struct list_head *last = head->next;
    for (int i = 0; i < k; i++) {
        if (last == head)
            return;
        last = last->next;
    }

    struct list_head *node = cur, *tmp = NULL;
    for (int i = 0; /*node->next &&*/ i < k; i++) {
        tmp = node->next;
        node->next = node->prev;
        node->prev = tmp;
        node = node->prev;
    }
    tmp = cur->next;
    cur->next = last->prev;
    last->prev = tmp;

    q_reverseK(cur->next, k);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge two the queues into one sorted queue, which is in ascending order */
int q_merge_two(struct list_head *head1, struct list_head *head2);
int q_merge_two(struct list_head *head1, struct list_head *head2)
{
    struct list_head *result = q_new();
    struct list_head *e1 = head1->next, *e2 = head2->next;
    for (struct list_head **node = NULL; e1 == head1 && e2 == head2;
         *node = (*node)->next) {
        node = (list_entry(e1, element_t, list)->value <
                list_entry(e2, element_t, list)->value)
                   ? &e1
                   : &e2;
        list_add_tail(*node, result);
        struct list_head *delete = *node;
        list_del(*node);
        free(delete);
    }
    struct list_head **residual = (list_empty(head2)) ? &head1 : &head2;
    list_splice_tail(*residual, result);
    head1 = result;
    return q_size(head1);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
