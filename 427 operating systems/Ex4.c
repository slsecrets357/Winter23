#include <stdio.h>
#include <stdlib.h>

int sort(int *arr, int len) {
    for (int i = 0; i < len; i++) {
        int maxidx = i;
        for (int j = i; j < len; j++) {
            if (arr[j] < arr[maxidx]) {
                maxidx = j;
            }
        }
        int tmp = arr[i];
        arr[i] = arr[maxidx];
        arr[maxidx] = tmp;
    }
}

int main(void) {

    int arr[16];
    for (int i = 0; i < 16; i++) {
        arr[i] = rand() % 120;
    }

    sort(arr, 16);

    for (int i = 0; i < 16; i++) {
        printf("%d, ", arr[i]);
    }
    printf("\n");
}