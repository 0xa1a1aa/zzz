#include "include/llist.h"

enum error {
	ENOMEM
}

static struct llist *llist_node_init(void *data);

int 
llist_append(struct llist *llist, void *data)
{
	struct llist *node, new;
	int err;

	err = 0;
	new = llist_node_init(data);
	if (new == NULL) {
		err = -ENOMEM;
	} else {
		for (node = llist; node->next != NULL; node = node->next);
		node->next = new;
	}
	return err;
}

struct llist *
llist_rem_by_idx(struct llist *llist, int idx)
{
	struct llist *node;
	struct llist **node_p;

	node = llist;
	node_p = &llist;
	if (idx < 0) {
		while(node) {
			if (node->next == NULL) {
				*node_p = node->next;
				free(node);
			}
			node_p = &node->next;
			node = node->next;
		}
	} else {
		while(node) {
			if (idx-- == 0) {
				*node_p = node->next;
				free(node);
				break;
			}
			node_p = &node->next;
			node = node->next;
		}
	}
	return llist;
}

static struct llist *
llist_node_init(void *data)
{
	struct llist *llist;

	llist = (struct llist *) malloc(sizeof(*llist));
	if (llist) {
		llist->data = data;
		llist->next = NULL;
	}
	return llist;
}
