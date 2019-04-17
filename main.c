#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>
#include <memory.h>

// Pseudocode from https://www.tutorialspoint.com/data_structures_
// algorithms/bubble_sort_algorithm.htm
//static const long Num_To_Sort = 1000000000;
static const long Num_To_Sort = 100000;
// Sequential version of your sort
// If you're implementing the PSRS algorithm, you may ignore this section
void sort_s(int *arr) {

    for(int i = 0; i < Num_To_Sort; i++){
        int swapped = 0; //0 == false, 1 == true
        //Compare against adjacent indexes
        for(int j = 0; j < Num_To_Sort; j++){
            if(arr[j] > arr[j+1]){//If left index is greater that right...
                int temp = arr[j +1];
                arr[j +1] = arr[j];
                arr[j] = temp;
                swapped = 1;
            }//End of if

        }// End of j for
        if(swapped == 0){//Nothing swapped.  Sorting complete
             i = Num_To_Sort; //Exit Loop
        }//End of if
    }//End of i for
}//End of sort_s

// Parallel version of your sort
void sort_p(int *arr) {
    #pragma omp parallel for schedule(static,1)
    for(int i = 0; i < Num_To_Sort; i++){
        int swapped = 0; //0 == false, 1 == true
        //Compare against adjacent indexes
        #pragma omp parallel for schedule(static,1)
        for(int j = 0; j < Num_To_Sort; j++){
            if(arr[j] > arr[j+1]){//If left index is greater that right...
                int temp = arr[j +1];
                arr[j +1] = arr[j];
                arr[j] = temp;
                swapped = 1;
            }//End of if

        }// End of j for
        if(swapped == 0){//Nothing swapped.  Sorting complete
            i = Num_To_Sort; //Exit Loop
        }//End of if
    }//End of i for
}

int main() {
    int *arr_s = malloc(sizeof(int) * Num_To_Sort);
    long chunk_size = Num_To_Sort / omp_get_max_threads();
#pragma omp parallel num_threads(omp_get_max_threads())
    {
        int p = omp_get_thread_num();
        unsigned int seed = (unsigned int) time(NULL) + (unsigned int) p;
        long chunk_start = p * chunk_size;
        long chunk_end = chunk_start + chunk_size;
        for (long i = chunk_start; i < chunk_end; i++) {
            arr_s[i] = rand_r(&seed);
        }
    }

    // Copy the array so that the sorting function can operate on it directly.
    // Note that this doubles the memory usage.
    // You may wish to test with slightly smaller arrays if you're running out of memory.
    int *arr_p = malloc(sizeof(int) * Num_To_Sort);
    memcpy(arr_p, arr_s, sizeof(int) * Num_To_Sort);

    struct timeval start, end;

    printf("Timing sequential...\n");
    gettimeofday(&start, NULL);
    sort_s(arr_s);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    free(arr_s);

    printf("Timing parallel...\n");
    gettimeofday(&start, NULL);
    sort_p(arr_p);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    free(arr_p);

    return 0;
}

