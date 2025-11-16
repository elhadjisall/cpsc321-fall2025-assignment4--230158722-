/*
 * Name: El Sall
 * Student Number: 230158722
 * GitHub Repository: https://github.com/elhadjisall/cpsc321-fall2025-assignment4--230158722-
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
void allocate_memory();  // Allocate memory for all data structures
void free_memory();      // Free all allocated memory
void read_input();       // Read system state from user input
void calculate_need();   // Calculate need matrix: need = maximum - allocation
bool is_safe(int *safe_sequence);  // Safety Algorithm: check if state is safe
bool request_resources(int customer_id, int *request, int *safe_sequence);  // Process resource request
bool validate_input();   // Validate input data for consistency

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
    if (total_resources == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free_memory();
        exit(1);
    }
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (scanf("%d", &total_resources[i]) != 1 || total_resources[i] < 0) {
            fprintf(stderr, "Invalid input\n");
            free(total_resources);
            free_memory();
            exit(1);
        }
    }
    
    printf("Enter available resources (%d values): ", NUMBER_OF_RESOURCES);
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (scanf("%d", &available[i]) != 1 || available[i] < 0) {
            fprintf(stderr, "Invalid input\n");
            free(total_resources);
            free_memory();
            exit(1);
        }
    }
    
    printf("Enter maximum demand matrix (%d x %d):\n", NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES);
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer %d: ", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            if (scanf("%d", &maximum[i][j]) != 1 || maximum[i][j] < 0) {
                fprintf(stderr, "Invalid input\n");
                free(total_resources);
                free_memory();
                exit(1);
            }
        }
    }
    
    printf("Enter current allocation matrix (%d x %d):\n", NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES);
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer %d: ", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            if (scanf("%d", &allocation[i][j]) != 1 || allocation[i][j] < 0) {
                fprintf(stderr, "Invalid input\n");
                free(total_resources);
                free_memory();
                exit(1);
            }
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

// Safety Algorithm: determines if the current system state is safe
// Returns true if a safe sequence exists, false otherwise
// If safe, safe_sequence will contain the execution order (C0, C1, ..., Cn-1)
// Algorithm:
//   1. Initialize work = available, finish[i] = false for all i
//   2. Find a customer i where finish[i] == false and need[i] <= work
//   3. If found: work = work + allocation[i], finish[i] = true, repeat step 2
//   4. If all customers finish, state is safe; otherwise unsafe
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

// Handle resource request from a customer using Banker's Algorithm
// Returns true if request is granted (leaves system in safe state), false if denied
// If granted, safe_sequence will contain the safe execution sequence
// Steps:
//   1. Check if request <= need[customer_id]
//   2. Check if request <= available
//   3. Check if allocation + request <= maximum
//   4. Temporarily allocate resources
//   5. Check if resulting state is safe using Safety Algorithm
//   6. If safe, keep allocation; if unsafe, revert allocation
bool request_resources(int customer_id, int *request, int *safe_sequence) {
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
    
    // Step 3: Check if allocation + request <= maximum
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        if (allocation[customer_id][j] + request[j] > maximum[customer_id][j]) {
            return false; // Request would exceed maximum
        }
    }
    
    // Step 4: Try to allocate resources temporarily
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        available[j] -= request[j];
        allocation[customer_id][j] += request[j];
        need[customer_id][j] -= request[j];
    }
    
    // Step 5: Check if resulting state is safe
    bool safe = is_safe(safe_sequence);
    
    if (!safe) {
        // Revert the allocation
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            available[j] += request[j];
            allocation[customer_id][j] -= request[j];
            need[customer_id][j] += request[j];
        }
        return false;
    }
    
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
    if (scanf("%d", &customer_id) != 1) {
        printf("State Unsafe\n");
        free_memory();
        return 0;
    }
    
    // Validate customer_id
    if (customer_id < 0 || customer_id >= NUMBER_OF_CUSTOMERS) {
        printf("State Unsafe\n");
        free_memory();
        return 0;
    }
    
    // Allocate memory for request and safe sequence
    int *request = (int*)malloc(NUMBER_OF_RESOURCES * sizeof(int));
    int *safe_sequence = (int*)malloc(NUMBER_OF_CUSTOMERS * sizeof(int));
    
    if (request == NULL || safe_sequence == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        if (request != NULL) free(request);
        if (safe_sequence != NULL) free(safe_sequence);
        free_memory();
        return 1;
    }
    
    // Read request values
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (scanf("%d", &request[i]) != 1) {
            printf("State Unsafe\n");
            free(request);
            free(safe_sequence);
            free_memory();
            return 0;
        }
        // Check for negative request values
        if (request[i] < 0) {
            printf("State Unsafe\n");
            free(request);
            free(safe_sequence);
            free_memory();
            return 0;
        }
    }
    
    // Process the request using request_resources function
    bool granted = request_resources(customer_id, request, safe_sequence);
    
    if (granted) {
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
        printf("State Unsafe\n");
    }
    
    free(request);
    free(safe_sequence);
    free_memory();
    
    return 0;
}

