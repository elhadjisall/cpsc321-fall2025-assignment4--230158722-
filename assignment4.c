#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Dynamic sizes - will be set based on input
int NUMBER_OF_CUSTOMERS;
int NUMBER_OF_RESOURCES;

/* the available amount of each resource */
int *available;

/* the maximum demand of each customer */
int **maximum;

/* the amount currently allocated to each customer */
int **allocation;

/* the remaining need of each customer */
int **need;

