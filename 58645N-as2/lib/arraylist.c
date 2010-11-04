// ArrayList class
#include <stdlib.h>

#include "arraylist.h"

int create(struct arli *list)
{
  list->size = 0;
  list->values = (int *) malloc (list->size * sizeof(int));

  if (list->values == NULL)
    return -1;

  return 0;
}

int add(struct arli *list, int number)
{
  int *tmp = list->values;

  list->size = list->size + 1;
  tmp = (int *) realloc(list->values, list->size * sizeof(int));

  if (tmp == NULL)
    return -1;

  list->values = tmp;
  list->values[list->size - 1] = number;

  return 0;
}

int get(struct arli list, int position)
{
  return list.values[position];
}

int eradicate(struct arli *list, int position)
{
  int *tmp, i = -1, j = 0;

  tmp = (int *) malloc ((list->size - 1) * sizeof(int));

  if (tmp == NULL)
    return -1;

  while (i < list->size)
  {
    i++;

    if (i == position)
      continue;

    tmp[j] = list->values[i];
    j++;
  }

  list->size = list->size - 1;
  free(list->values);
  list->values = tmp;

  return 0;
}

void destroy(struct arli *list)
{
  free(list->values);
  list->size = 0;
}

