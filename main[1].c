
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define N 100 // Number of integers to sort
#define M 4   // Number of child processes

void generate_numbers(int *numbers, int n);
void print_numbers(int *numbers, int n);
void merge(int *array, int left, int mid, int right);
void bubble_sort(int *array, int size);
void merge_sorted(int *array, int segment_size, int num_segments);

int main() {
    int numbers[N];
    int portion_size = N / M;
    int pipes[M][2]; // [0] for reading and [1] for writing
    pid_t pids[M];

    generate_numbers(numbers, N);
    printf("unsorted array:\n");
    print_numbers(numbers, N);

    // Create pipes
    for (int i = 0; i < M; i++) {
        if (pipe(pipes[i]) == -1) {
            printf("pipe");
            return 1;
        }
    }

    // Create M child processes
    for (int i = 0; i < M; i++) {
        if ((pids[i] = fork()) == 0) { // Child process
            close(pipes[i][0]); // Close read end
// Calculate the start and end indices of the portion to be sorted
            int start = i * portion_size;
            int end = (i == M - 1) ? N : start + portion_size;
            int size = end - start;
            //child process portion using bubblesort
            bubble_sort(numbers + start, size);

            // Send sorted segment back to parent
            write(pipes[i][1], numbers + start, size * sizeof(int));
            close(pipes[i][1]); // Close write end
            exit(0);
        } else if (pids[i] < 0) {
            printf("fork");
            return 2;
        }
    }

    // Parent process
    for (int i = 0; i < M; i++) {
        close(pipes[i][1]); // Close write end
    }

    // Wait for all child processes to finish
    for (int i = 0; i < M; i++) {
        wait(NULL);
    }

    // Read sorted part from pipes
    for (int i = 0; i < M; i++) {
        int start = i * portion_size;
        int end = (i == M - 1) ? N : start + portion_size;
        int size = end - start;
        read(pipes[i][0], numbers + start, size * sizeof(int));
        close(pipes[i][0]); // Close read end
    }

    // Merge sorted part
    merge_sorted(numbers, portion_size, M);

    printf("Sorted array:\n");
    print_numbers(numbers, N);

    return 0;
}
// create random numbers
void generate_numbers(int *numbers, int n) {

    for (int i = 0; i < n; i++) {
        numbers[i] = rand() % 1000;
    }
}

void print_numbers(int *numbers, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");
}

void bubble_sort(int *array, int size) {
    // Iterate through the array from the beginning to the second last element
    for (int i = 0; i < size - 1; i++) {
         // For each element, compare with subsequent elements until the sorted part

        for (int j = 0; j < size - i - 1; j++) {
         // If the current element is greater than the next element, swap them
   
            if (array[j] > array[j + 1]) {
                //swap
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}
// Function to merge two sorted arrays
void merge(int *array, int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;// Calculate the size of the left subarray
    int n2 = right - mid;// Calculate the size of the right subarray

    int *L = (int *)malloc(n1 * sizeof(int));// Allocate memory for the left subarray
    int *R = (int *)malloc(n2 * sizeof(int));// Allocate memory for the right subarray
  // Copy data to temporary arrays L[] and R[]
    for (i = 0; i < n1; i++) {
        L[i] = array[left + i];
    }
    for (j = 0; j < n2; j++) {
        R[j] = array[mid + 1 + j];
    }

    i = 0; // Initial index of the left 
    j = 0;//  index right
    k = left;//Initial index of the merged array
     // Merge the two subarrays into the original array
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            array[k] = L[i];//Copy the smaller element from the left subarray
            i++; // Move to the next element in the left subarray
        } else {
            array[k] = R[j];
            j++;
        }
        k++;
    }
// Copy the remaining elements of L[], if there are any
    while (i < n1) {
        array[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        array[k] = R[j];
        j++;
        k++;
    }

    free(L); // Free the left subarray memory
    free(R);  // Free the left subarray memory
}

void merge_sorted(int *array, int segment_size, int num_segments) {
    int current_size = segment_size;
    while (current_size < N) {
        for (int i = 0; i < num_segments; i += 2) {
            int left = i * current_size;
            int mid = left + current_size - 1;
            int right = ((i + 2) * current_size) - 1;
            if (right >= N) {
                right = N - 1;
            }
            merge(array, left, mid, right);
        }
        current_size *= 2;
    }
}
