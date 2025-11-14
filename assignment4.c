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

// Function prototypes
void allocate_memory();
void free_memory();
void read_input();
void calculate_need();
bool is_safe(int *safe_sequence);

// Allocate memory for all data structures
void allocate_memory() {
    available = (int*)malloc(NUMBER_OF_RESOURCES * sizeof(int));
    
    maximum = (int**)malloc(NUMBER_OF_CUSTOMERS * sizeof(int*));
    allocation = (int**)malloc(NUMBER_OF_CUSTOMERS * sizeof(int*));
    need = (int**)malloc(NUMBER_OF_CUSTOMERS * sizeof(int*));
    
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        maximum[i] = (int*)malloc(NUMBER_OF_RESOURCES * sizeof(int));
        allocation[i] = (int*)malloc(NUMBER_OF_RESOURCES * sizeof(int));
        need[i] = (int*)malloc(NUMBER_OF_RESOURCES * sizeof(int));
    }
}

// Free all allocated memory
void free_memory() {
    free(available);
    
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        free(maximum[i]);
        free(allocation[i]);
        free(need[i]);
    }
    
    free(maximum);
    free(allocation);
    free(need);
}

// Read input from user
void read_input() {
    printf("Enter number of customers (processes): ");
    scanf("%d", &NUMBER_OF_CUSTOMERS);
    
    printf("Enter number of resources: ");
    scanf("%d", &NUMBER_OF_RESOURCES);
    
    allocate_memory();
    
    printf("Enter number of instances of each resource type (%d values): ", NUMBER_OF_RESOURCES);
    int *total_resources = (int*)malloc(NUMBER_OF_RESOURCES * sizeof(int));
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        scanf("%d", &total_resources[i]);
    }
    
    printf("Enter available resources (%d values): ", NUMBER_OF_RESOURCES);
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        scanf("%d", &available[i]);
    }
    
    printf("Enter maximum demand matrix (%d x %d):\n", NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES);
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer %d: ", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            scanf("%d", &maximum[i][j]);
        }
    }
    
    printf("Enter current allocation matrix (%d x %d):\n", NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES);
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer %d: ", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            scanf("%d", &allocation[i][j]);
        }
    }
    
    free(total_resources);
}

// Calculate need matrix: need[i][j] = maximum[i][j] - allocation[i][j]
void calculate_need() {
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }
}

// Safety algorithm: returns true if system is in safe state
// safe_sequence will contain the safe sequence if found
bool is_safe(int *safe_sequence) {
    int *work = (int*)malloc(NUMBER_OF_RESOURCES * sizeof(int));
    bool *finish = (bool*)malloc(NUMBER_OF_CUSTOMERS * sizeof(bool));
    
    // Initialize work = available
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        work[i] = available[i];
    }
    
    // Initialize finish[i] = false for all i
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        finish[i] = false;
    }
    
    int count = 0;
    bool found;
    
    // Find a customer i such that finish[i] == false and need[i] <= work
    while (count < NUMBER_OF_CUSTOMERS) {
        found = false;
        
        for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
            if (!finish[i]) {
                // Check if need[i] <= work
                bool can_allocate = true;
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                    if (need[i][j] > work[j]) {
                        can_allocate = false;
                        break;
                    }
                }
                
                if (can_allocate) {
                    // Customer i can complete: work = work + allocation[i]
                    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true;
                    safe_sequence[count] = i;
                    count++;
                    found = true;
                    break;
                }
            }
        }
        
        // If no customer found, system is unsafe
        if (!found) {
            free(work);
            free(finish);
            return false;
        }
    }
    
    free(work);
    free(finish);
    return true;
}

