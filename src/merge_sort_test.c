#include <stdio.h>
#include <stdlib.h>

#define LEN 60

void test_merge_sort_merge(int *a, int *b, int start, int middle, int end) {
	int start_i = start;
	int middle_i = middle;

	for(int k = start; k < end; k++) {
		if(start_i < middle && (middle_i >= end || a[start_i] <= a[middle_i])) {
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

void test_merge_sort_split(int *a, int *b, int start, int end) {
	if((end - start) <= 1) return;
	int middle = (start + end) / 2;
	test_merge_sort_split(a, b, middle, end);
	test_merge_sort_split(a, b, start, middle);
	test_merge_sort_merge(b, a, start, middle, end);
}

void test_merge_sort(int *t, int *scratch, int len) {
	for(int i = 0; i < len; i++)
		scratch[i] = t[i];
	test_merge_sort_split(scratch, t, 0, len);
}

int random_num(int min, int max) {
	return (int)(min + (max - min) * ((float)rand() / RAND_MAX));
}

// int main() {
// 
// 	int nums[LEN];
// 	int scratch[LEN];
// 
// 	for(int i = 0; i < 10; i++) {
// 		printf("Before sorting: \n");
// 		for(int j = 0; j < LEN; j++) {
// 			nums[j] = random_num(0, LEN);
// 			printf("%d ", nums[j]);
// 		}
// 		printf("\n");
// 		test_merge_sort(nums, scratch, LEN);
// 
// 		printf("After sorting: \n");
// 		for(int j = 0; j < LEN; j++) {
// 			printf("%d ", nums[j]);
// 		}
// 		printf("\n");
// 		printf("\n");
// 	}
// }
