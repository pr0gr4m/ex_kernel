#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "list.h"

int main(void)
{
	struct item * item1 = (struct item *)malloc(sizeof(struct item));
	struct item * item2 = (struct item *)malloc(sizeof(struct item));
	struct item * item3 = (struct item *)malloc(sizeof(struct item));
	struct item *current = NULL;

	memset(item1, 0, sizeof(struct item));
	memset(item2, 0, sizeof(struct item));
	memset(item3, 0, sizeof(struct item));

	item1->number = 100;
	item2->number = 200;
	item3->number = 300;

	list_add(&item1->list, &head);
	list_add(&item2->list, &head);
	list_add(&item3->list, &head);

	for_each_item(current)
	{
		printf("Number: %d\n", current->number);
	}

	list_del(&item1->list);
	list_del(&item2->list);
	list_del(&item3->list);

	free(item1);
	free(item2);
	free(item3);

	return 0;
}
