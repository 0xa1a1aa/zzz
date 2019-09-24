#ifndef __LLIST__
#define __LLIST__

enum llist_error {
	ENOMEM
};

/*
 * Data destructor function
 */
typedef void (*data_dtor_t)(void *data);

struct llist {
	void *data;
	struct llist *next;
};

/*
 * Create a new linked list
 *
 * @params:
 * 	data: pointer to data for the first node
 * @returns:
 * 	pointer to llist struct
 */
struct llist *llist_init(void *data);

/*
 * Destroy a linked list
 *
 * @params:
 * 	llist: linked list to destroy
 * 	data_dtor: Data destructor function which gets called for every llist node
 * @returns;
 * 	-
 */
void llist_destroy(struct llist *llist, data_dtor_t data_dtor);

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
 * Get a llist node by index
 *
 * @params:
 * 	llist: linked list
 * 	idx: index of node
 * @returns:
 * 	pointer to llist or NULL
 */
struct llist *llist_get_by_idx(struct llist *llist, int idx);

/*
 * Remove node of linked list by index
 *
 * @params:
 * 	llist: pointer to linked list where node is to remove
 * 	idx: index of node to remove
 * 		- first node has index 0
 * 		- if idx is less than 0, the last node gets removed
 * 	data: the pointer to the data from the removed node gets stored here
 * @returns:
 * 	pointer to new linked list head
 */
struct llist *llist_rem_by_idx(struct llist *llist, int idx, void **data);

#endif /* __LLIST__ */
