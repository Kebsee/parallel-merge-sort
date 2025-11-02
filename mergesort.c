#include <stdio.h>
#include <string.h> 
#include <stdlib.h> /* For malloc, free */
#include "mergesort.h"

/* Merge two sorted ranges [leftstart..leftend] and [rightstart..rightend]
 * Using global arrays A (source) and B (temp). */
void merge(int leftstart, int leftend, int rightstart, int rightend){
    int i = leftstart;
    int j = rightstart;
    int k = leftstart;
    int p; 

    while (i <= leftend && j <= rightend) {
        if (A[i] <= A[j]) {
            B[k++] = A[i++];
        } else {
            B[k++] = A[j++];
        }
    }
    while (i <= leftend) B[k++] = A[i++];
    while (j <= rightend) B[k++] = A[j++];

    /* Copy merged range back into A */
    for (p = leftstart; p <= rightend; ++p) {
        A[p] = B[p];
    }
}

/* Standard recursive serial mergesort on A using B as temporary storage */
void my_mergesort(int left, int right){
    int mid; 
    if (left >= right) return;
    mid = left + (right - left) / 2;
    my_mergesort(left, mid);
    my_mergesort(mid + 1, right);
    merge(left, mid, mid + 1, right);
}

/* Parallel mergesort:
 * - Spawns up to 'cutoff' levels of threads; beyond that uses serial mergesort.
 * - Argument ownership: arguments created via buildArgs() are freed by the
 *   callee when a->level > 0. The level-0 arg (owned by harness) is never freed here. */
void * parallel_mergesort(void *arg){
    struct argument *a = (struct argument *)arg;
    if (!a) return NULL;

    /* Base cases */
    if (a->left >= a->right) {
        /* Nothing to sort */
        if (a->level > 0) free(a);
        return NULL;
    }

    /* If we've reached the cutoff (no more thread creation), do serial mergesort */
    if (a->level >= cutoff) {
        my_mergesort(a->left, a->right);
        if (a->level > 0) free(a);
        return NULL;
    }

    /* Otherwise split work and spawn two threads (or run directly on failure) */
    int left = a->left;
    int right = a->right;
    int mid = left + (right - left) / 2;

    /* Prepare child arguments (allocated) */
    struct argument *leftArg = buildArgs(left, mid, a->level + 1);
    struct argument *rightArg = buildArgs(mid + 1, right, a->level + 1);
    if (!leftArg || !rightArg) {
        /* Allocation failed: fallback to serial to preserve correctness */
        if (leftArg) free(leftArg);
        if (rightArg) free(rightArg);
        my_mergesort(left, right);
        if (a->level > 0) free(a);
        return NULL;
    }

    pthread_t t1, t2;
    int created1 = 0, created2 = 0;
    int rc;

    // Pthread returns 0 on success

    rc = pthread_create(&t1, NULL, parallel_mergesort, leftArg);
    if (rc == 0) {
        created1 = 1;
    } else {
        /* Creation failed: run in current thread (parallel_mergesort frees leftArg) */
        perror("pthread_create");
        parallel_mergesort(leftArg);
    }

    rc = pthread_create(&t2, NULL, parallel_mergesort, rightArg);
    if (rc == 0) {
        created2 = 1;
    } else {
        perror("pthread_create");
        parallel_mergesort(rightArg);
    }
    // If not 0
    if (created1) { 
        if (pthread_join(t1, NULL) != 0) {
            perror("pthread_join");
            /* Continue anyway to attempt merge */
        }
    }
    if (created2) {
        if (pthread_join(t2, NULL) != 0) {
            perror("pthread_join");
        }
    }

    /* Now both halves are sorted, merge them */
    merge(left, mid, mid + 1, right);

    /* Free this invocation's argument if it was dynamically allocated for a spawned thread */
    if (a->level > 0) free(a);
    return NULL;
}

/* Allocate and return an argument struct for parallel_mergesort */
struct argument * buildArgs(int left, int right, int level){
    struct argument *a = malloc(sizeof(struct argument));
    if (!a) return NULL;
    a->left = left;
    a->right = right;
    a->level = level;
    return a;
}
