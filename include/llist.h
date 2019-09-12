#ifndef __LLIST__
#define __LLIST__

struct llist {
	void *data;
	struct llist *next;
}

/*
 * Append a new node to a linked list
 *
 * @params:
 * 	llist: linked list where to append the new node
 * 	data:  pointer to data of the new node
 * @returns;
 * 	0 on success
 * 	-1 on failure
 * 		- the only reason for failure is that there
 * 		is not enough memory to allocate a llist struct
 */
int llist_append(struct llist *llist, void *data);

/*
 * Remove node of linked list by index
 *
 * @params:
 * 	llist: pointer to linked list where node is to remove
 * 	idx: index of node to remove
 * 		- first node has index 0
 * 		- if idx is less than 0, the last node gets removed
 * @returns:
 * 	pointer to new linked list head
 */
struct llist *llist_rem_by_idx(struct llist *llist, int idx);

#endif /* __LLIST__ */
