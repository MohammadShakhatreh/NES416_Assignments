#include <stdio.h>

#include "utils.h"

// Counts the number of digits in a number
int digit_cnt(int x) {

  int cnt = 0;
  while(x > 0) {
    x /= 10;
    ++cnt;
  }

  return cnt;
}

void stoa(const char *str, int *arr, size_t *len) {

	// Pointer to walk through the array
	char *ptr = str;

	// Get the length of the array
	sscanf(ptr, "%d", len);

	// Go to the first element in the array
	ptr += digit_cnt(len) + 1;

	arr = malloc(*len * sizeof(int));
	for(int i = 0; i < *len; ++i) {
		sscanf(ptr, "%d", arr + i);
		ptr += digit_cnt(arr[i]) + 1;
	}
}

void atos(const int *arr, size_t len, char *str) {

	// Pointer to walk through the string
	char *ptr = str;

	for(int i = 0; i < len; ++i) {
		sprintf(str, "%d%s", arr[i], i != 0 ? " " : "");
		ptr += digit_cnt(arr[i]) + 1;
	}
}