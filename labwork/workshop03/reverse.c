#include <stdlib.h>
#include "reverse.h"

int string_length(const char *str){
    int i = 0;
    while(str[i]) i++;
    return i;
}

char *reverse(const char *str){
    int len = string_length(str);
    char * new_str = (char *)malloc(len * sizeof(char));
    for(int i = 0; i < len; i++){
        *(new_str+i) = *(str+len-i-1);
    }
    return new_str;
}