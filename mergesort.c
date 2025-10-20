#include <stdio.h>
#include <string.h> /* for memcpy */
#include <stdlib.h> /* for malloc, free */
#include "mergesort.h"

/* merge two sorted ranges [leftstart..leftend] and [rightstart..rightend]
 * using global arrays A (source) and B (temp). */
void merge(int leftstart, int leftend, int rightstart, int rightend){
    int i = leftstart;
    int j = rightstart;
    int k = leftstart;
    int p; /* declare outside the for-loop for gnu89 compatibility */

    while (i <= leftend && j <= rightend) {
        if (A[i] <= A[j]) {
            B[k++] = A[i++];
        } else {
            B[k++] = A[j++];
        }
    }
    while (i <= leftend) B[k++] = A[i++];
    while (j <= rightend) B[k++] = A[j++];

    /* copy merged range back into A */
    for (p = leftstart; p <= rightend; ++p) {
        A[p] = B[p];
    }
}

/* standard recursive serial mergesort on A using B as temporary storage */
void my_mergesort(int left, int right){
    int mid; /* declare at top for gnu89 */
    if (left >= right) return;
    mid = left + (right - left) / 2;
    my_mergesort(left, mid);
    my_mergesort(mid + 1, right);
    merge(left, mid, mid + 1, right);
}

/* basic parallel entry: here we just call serial mergesort so test program can
 * exercise serial behaviour (use cutoff==0 with test-mergesort to get pure serial).
 * A fuller parallel implementation can be added later. */
// ...existing code...
void * parallel_mergesort(void *arg){
    struct argument *a = (struct argument *)arg;
    if (!a) return NULL;
    my_mergesort(a->left, a->right);
    /* only free if this arg was allocated for a spawned thread (level > 0) */
    if (a->level > 0) free(a);
    return NULL;
}

/* allocate and return an argument struct for parallel_mergesort */
struct argument * buildArgs(int left, int right, int level){
    struct argument *a = malloc(sizeof(struct argument));
    if (!a) return NULL;
    a->left = left;
    a->right = right;
    a->level = level;
    return a;
}