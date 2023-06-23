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
    if (!q)  // If allocate failed
        return NULL;
    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        list_del(&entry->list);
        q_release_element(entry);
    }
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

    if (list_empty(head))
        return q_insert_head(head, s);
    else
        return q_insert_head(head->prev, s);
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;
    element_t *target = list_first_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, target->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
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
        strncpy(sp, target->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&target->list);
    return target;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
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

    if (!head || list_empty(head))
        return false;
    struct list_head *front = head->next, *back = head->prev;
    while (front != back && front->next != back) {
        front = front->next;
        back = back->prev;
    }

    list_del(front);
    q_release_element(list_entry(front, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    if (!head || list_empty(head))
        return false;

    element_t *curr, *next;
    bool dup = 0;
    list_for_each_entry_safe (curr, next, head, list) {
        if (&next->list != head && strcmp(curr->value, next->value) == 0) {
            list_del(&curr->list);
            q_release_element(curr);
            dup = 1;
        } else if (dup) {
            list_del(&curr->list);
            q_release_element(curr);
            dup = 0;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/

    if (!head || list_empty(head))
        return;

    for (struct list_head *node = head->next;
         node != head && node->next != head; node = node->next) {
        struct list_head *tmp = node;
        list_move(node, tmp->next);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node, *safe, *tmp;
    list_for_each_safe (node, safe, head) {
        tmp = node->next;
        node->next = node->prev;
        node->prev = tmp;
    }
    tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/

    if (!head || list_empty(head))
        return;

    int count = 0;
    struct list_head sub_q, *node, *safe, *tmp = head;
    list_for_each_safe (node, safe, head) {
        count++;
        if (count == k) {
            INIT_LIST_HEAD(&sub_q);
            list_cut_position(&sub_q, tmp, node);
            q_reverse(&sub_q);
            list_splice_init(&sub_q, tmp);
            count = 0;
            tmp = safe->prev;
        }
    }
}

/* Merge two the queues into one sorted queue, which is in ascending order */
void q_merge_two(struct list_head *q1, struct list_head *q2, bool descend);
void q_merge_two(struct list_head *q1, struct list_head *q2, bool descend)
{
    if (!q1 || !q2)
        return;
    struct list_head *result = q_new();
    element_t *node;
    if (descend) {
        while (!list_empty(q1) && !list_empty(q2)) {
            element_t *e1 = list_first_entry(q1, element_t, list);
            element_t *e2 = list_first_entry(q2, element_t, list);
            node = (strcmp(e1->value, e2->value) > 0) ? e1 : e2;
            list_move_tail(&node->list, result);
        }
    } else {
        while (!list_empty(q1) && !list_empty(q2)) {
            element_t *e1 = list_first_entry(q1, element_t, list);
            element_t *e2 = list_first_entry(q2, element_t, list);
            node = (strcmp(e1->value, e2->value) < 0) ? e1 : e2;
            list_move_tail(&node->list, result);
        }
    }

    struct list_head *residual = (list_empty(q2)) ? q1 : q2;
    list_splice_tail_init(residual, result);
    list_splice(result, q1);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || head->next == head->prev)
        return;

    struct list_head *mid = head;
    for (struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next)
        mid = mid->next;

    struct list_head *left = q_new();
    list_cut_position(left, head, mid);
    // head will become right half
    q_merge(left, descend);
    q_merge(head, descend);
    q_merge_two(head, left, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    element_t *curr = list_entry(head->prev, element_t, list);
    while (curr->list.prev != head) {
        element_t *next = list_entry(curr->list.prev, element_t, list);
        if (strcmp(next->value, curr->value) > 0) {
            list_del(&next->list);
            q_release_element(next);
        } else {
            curr = next;
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    element_t *curr = list_entry(head->prev, element_t, list);
    while (curr->list.prev != head) {
        element_t *next = list_entry(curr->list.prev, element_t, list);
        if (strcmp(next->value, curr->value) < 0) {
            list_del(&next->list);
            q_release_element(next);
        } else {
            curr = next;
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    if (head->next == head->prev)
        return list_first_entry(head, queue_contex_t, chain)->size;

    queue_contex_t *first = list_first_entry(head, queue_contex_t, chain);
    queue_contex_t *curr, *safe;

    list_for_each_entry_safe (curr, safe, head, chain) {
        if (curr != first) {
            q_merge_two(first->q, curr->q, descend);
            curr->q = NULL;
        }
    }

    return q_size(first->q);
}
