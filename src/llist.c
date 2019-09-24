#include <stdlib.h>
#include "llist.h"

static struct llist *llist_node_init(void *data);

struct llist *
llist_init(void *data)
{
	return llist_node_init(data);
}

void
llist_destroy(struct llist *llist, data_dtor_t data_dtor)
{
	void *data;

	while (llist != NULL) {
		llist = llist_rem_by_idx(llist, 0, &data);
		if (data_dtor) {
			data_dtor(data);
		}
	}
}

int 
llist_append(struct llist *llist, void *data)
{
	struct llist *node, *new;
	int err;

	err = 0;
	new = llist_node_init(data);
	if (new != NULL) {
		for (node = llist; node->next != NULL; node = node->next);
		node->next = new;
	} else {
		err = ENOMEM;
	}
	return err;
}

struct llist *
llist_get_by_idx(struct llist *llist, int idx)
{
	struct llist *entry;

	entry = NULL;
	if (idx < 0) {
		while(llist) {
			if (llist->next == NULL) {
				entry = llist;
				break;
			}
			llist = llist->next;
		}
	} else {
		while(llist) {
			if (idx-- == 0) {
				entry = llist;
				break;
			}
			llist = llist->next;
		}
	}
	return entry;
}

struct llist *
llist_rem_by_idx(struct llist *llist, int idx, void **data)
{
	struct llist *node;
	struct llist **node_p;

	node = llist;
	node_p = &llist;
	if (idx < 0) {
		while(node) {
			if (node->next == NULL) {
				*node_p = NULL;
				*data = node->data;
				free(node);
				break;
			}
			node_p = &node->next;
			node = node->next;
		}
	} else {
		while(node) {
			if (idx-- == 0) {
				*node_p = node->next;
				*data = node->data;
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
