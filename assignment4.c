/*
 * Banker's Algorithm Implementation
 * 
 * This program implements the Banker's Algorithm for deadlock avoidance.
 * It reads system state (available resources, maximum demand, current allocation)
 * and processes resource requests, determining if granting the request would
 * leave the system in a safe state.
 * 
 * Input: System state and resource request
 * Output: "State Safe" with safe sequence, or "State Unsafe"
 */

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
bool request_resources(int customer_id, int *request);
bool validate_input();

// Allocate memory for all data structures
void allocate_memory() {
    available = (int*)malloc(NUMBER_OF_RESOURCES * sizeof(int));
    if (available == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    maximum = (int**)malloc(NUMBER_OF_CUSTOMERS * sizeof(int*));
    allocation = (int**)malloc(NUMBER_OF_CUSTOMERS * sizeof(int*));
    need = (int**)malloc(NUMBER_OF_CUSTOMERS * sizeof(int*));
    
    if (maximum == NULL || allocation == NULL || need == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        maximum[i] = (int*)malloc(NUMBER_OF_RESOURCES * sizeof(int));
        allocation[i] = (int*)malloc(NUMBER_OF_RESOURCES * sizeof(int));
        need[i] = (int*)malloc(NUMBER_OF_RESOURCES * sizeof(int));
        
        if (maximum[i] == NULL || allocation[i] == NULL || need[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
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
    if (scanf("%d", &NUMBER_OF_CUSTOMERS) != 1 || NUMBER_OF_CUSTOMERS <= 0) {
        fprintf(stderr, "Invalid input\n");
        exit(1);
    }
    
    printf("Enter number of resources: ");
    if (scanf("%d", &NUMBER_OF_RESOURCES) != 1 || NUMBER_OF_RESOURCES <= 0) {
        fprintf(stderr, "Invalid input\n");
        exit(1);
    }
    
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

// Validate input data for consistency
bool validate_input() {
    // Check for negative values
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (available[i] < 0) {
            return false;
        }
    }
    
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            if (maximum[i][j] < 0 || allocation[i][j] < 0) {
                return false;
            }
            // Allocation should not exceed maximum
            if (allocation[i][j] > maximum[i][j]) {
                return false;
            }
        }
    }
    
    // Check that need matrix is non-negative (calculated after)
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            if (need[i][j] < 0) {
                return false;
            }
        }
    }
    
    return true;
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
    
    if (work == NULL || finish == NULL) {
        if (work != NULL) free(work);
        if (finish != NULL) free(finish);
        return false;
    }
    
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

// Handle resource request from a customer
// Returns true if request is granted, false if denied
bool request_resources(int customer_id, int *request) {
    // Step 1: Check if request <= need[customer_id]
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        if (request[j] > need[customer_id][j]) {
            return false; // Request exceeds need
        }
    }
    
    // Step 2: Check if request <= available
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        if (request[j] > available[j]) {
            return false; // Request exceeds available
        }
    }
    
    // Step 3: Try to allocate resources temporarily
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        available[j] -= request[j];
        allocation[customer_id][j] += request[j];
        need[customer_id][j] -= request[j];
    }
    
    // Step 4: Check if resulting state is safe
    int *safe_sequence = (int*)malloc(NUMBER_OF_CUSTOMERS * sizeof(int));
    bool safe = is_safe(safe_sequence);
    
    if (!safe) {
        // Revert the allocation
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            available[j] += request[j];
            allocation[customer_id][j] -= request[j];
            need[customer_id][j] += request[j];
        }
        free(safe_sequence);
        return false;
    }
    
    free(safe_sequence);
    return true;
}

int main() {
    // Read initial system state
    read_input();
    
    // Calculate need matrix
    calculate_need();
    
    // Validate input data
    if (!validate_input()) {
        printf("State Unsafe\n");
        free_memory();
        return 0;
    }
    
    // Read resource request
    printf("Enter Resource Request: ");
    int customer_id;
    scanf("%d", &customer_id);
    
    // Validate customer_id
    if (customer_id < 0 || customer_id >= NUMBER_OF_CUSTOMERS) {
        printf("State Unsafe\n");
        free_memory();
        return 0;
    }
    
    int *request = (int*)malloc(NUMBER_OF_RESOURCES * sizeof(int));
    if (request == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free_memory();
        return 1;
    }
    
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (scanf("%d", &request[i]) != 1) {
            printf("State Unsafe\n");
            free(request);
            free_memory();
            return 0;
        }
        // Check for negative request values
        if (request[i] < 0) {
            printf("State Unsafe\n");
            free(request);
            free_memory();
            return 0;
        }
    }
    
    // Check if request is valid and process it
    // Step 1: Check if request <= need[customer_id] (ensures allocation + request <= maximum)
    // Step 2: Check if request <= available
    bool valid = true;
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        if (request[j] > need[customer_id][j] || request[j] > available[j]) {
            valid = false;
            break;
        }
        // Additional check: ensure allocation + request doesn't exceed maximum
        if (allocation[customer_id][j] + request[j] > maximum[customer_id][j]) {
            valid = false;
            break;
        }
    }
    
    if (!valid) {
        printf("State Unsafe\n");
        free(request);
        free_memory();
        return 0;
    }
    
    // Temporarily apply the request
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        available[j] -= request[j];
        allocation[customer_id][j] += request[j];
        need[customer_id][j] -= request[j];
    }
    
    // Check if resulting state is safe
    int *safe_sequence = (int*)malloc(NUMBER_OF_CUSTOMERS * sizeof(int));
    if (safe_sequence == NULL) {
        // Revert the request
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            available[j] += request[j];
            allocation[customer_id][j] -= request[j];
            need[customer_id][j] += request[j];
        }
        printf("State Unsafe\n");
        free(request);
        free_memory();
        return 1;
    }
    
    bool safe = is_safe(safe_sequence);
    
    if (safe) {
        printf("State Safe\n");
        printf("Safe sequence: ");
        for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
            printf("C%d", safe_sequence[i]);
            if (i < NUMBER_OF_CUSTOMERS - 1) {
                printf(" ");
            }
        }
        printf("\n");
    } else {
        // Revert the request
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            available[j] += request[j];
            allocation[customer_id][j] -= request[j];
            need[customer_id][j] += request[j];
        }
        printf("State Unsafe\n");
    }
    
    free(request);
    free(safe_sequence);
    free_memory();
    
    return 0;
}

