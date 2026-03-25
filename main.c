#include <stdio.h>
#include "dynamic_array.h"

void int_array_print(IntArray arr){
    printf("[%zu/%zu ", arr.len, arr.cap);
    for(int i = 0; i < arr.len; i++){
        printf("%d ", arr.v[i]);
    }
    printf("]\n");
}

int main(){
    IntArray arr = {0};
    int_array_init(&arr, 0, 32);

    for(int i = 0; i < 65; i++){
        int_array_append(&arr, i * 2);
        int_array_print(arr);
    }

}
