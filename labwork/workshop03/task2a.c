#include <stdio.h>
#include <stdlib.h>

void print(int *arr, int n);

int main(int argc, char * argv[]){

    int *t1 = (int *)malloc(5 * sizeof(int));
    if(t1 == NULL){
        exit(1);
    }
    for(int i = 0; i < 5; i++){
        *(t1+i) = i;
    }
    print(t1,5);
    free(t1);
    t1 = NULL;
    return 0; 
}

void print(int *arr, int n){
    for(int i = 0; i < n; i++){
        printf("[%d] %d\n", i, arr[i]);
    }
}