#include "triangle.h"
#include <stdio.h>
#include <stdlib.h>

Face cube_faces[12] = {
    { .a = 1, .b = 2, .c = 3},
    { .a = 1, .b = 3, .c = 4},

    { .a = 4, .b = 3, .c = 5},
    { .a = 4, .b = 5, .c = 6},

    { .a = 6, .b = 5, .c = 7},
    { .a = 6, .b = 7, .c = 8},

    { .a = 8, .b = 7, .c = 2},
    { .a = 8, .b = 2, .c = 1},

    { .a = 2, .b = 7, .c = 5},
    { .a = 2, .b = 5, .c = 3},

    { .a = 6, .b = 8, .c = 1},
    { .a = 6, .b = 1, .c = 4}
};

float sort_prop(Triangle t) {
	return t.avg_depth;
}

void merge_sort_merge(Triangle *a, Triangle *b, int start, int middle, int end) {
	int start_i = start;
	int middle_i = middle;

	for(int k = start; k < end; k++) {
		if(start_i < middle && (middle_i >= end || (sort_prop(a[start_i]) <= sort_prop(a[middle_i])))) {
			b[k] = a[start_i];
			start_i++;
		} else {
			b[k] = a[middle_i];
			middle_i++;
		}
	}

	for(int i = start; i < end; i++)
		a[i] = b[i];
}

void merge_sort_split(Triangle *a, Triangle *b, int start, int end) {
	if((end - start) <= 1) return;
	int middle = (start + end) / 2;
	merge_sort_split(a, b, middle, end);
	merge_sort_split(a, b, start, middle);
	merge_sort_merge(b, a, start, middle, end);
}

void merge_sort_triangles(Triangle *t, int len) {
	Triangle *scratch = (Triangle *) malloc(sizeof(Triangle) * len);
	for(int i = 0; i < len; i++)
		scratch[i] = t[i];
	merge_sort_split(scratch, t, 0, len);
}

