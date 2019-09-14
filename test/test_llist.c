#include <stdio.h>
#include <string.h>
#include "llist.h"

void print_llist(struct llist* llist);

int
main(void)
{
	struct llist *head, *node;
	char *data1 = "test data 1";
	char *data2 = "test data 2";
	char *data3 = "test data 3";

	printf("==========[ TESTING: llist ]==========\n");

	printf("llist_init: ");
	head = llist_init((void *)data1);
	if (strcmp(data1, head->data)) {
		printf("ERROR\n");
	} else {
		printf("OK\n");
	}

	printf("llist_append: ");
	llist_append(head, (void *)data2);
	llist_append(head, (void *)data3);
	node = head->next;
	if (strcmp(data2, node->data)) {
		printf("ERROR\n");
	} else {
		printf("OK\n");
	}

	printf("llist_rem_by_idx: ");
	node = head->next;
	head = llist_rem_by_idx(head, 0);
	if (head != node) {
		printf("ERROR\n");
	} else {
		node = head;
		head = llist_rem_by_idx(head, -1);
		if (head != node) {
			printf("ERROR\n");
		} else {
			printf("OK\n");
		}
	}

	// llist_destroy cant fail unless llist_rem_by_idx fails
	llist_destroy(head);

	return 0;
}
