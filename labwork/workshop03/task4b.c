#include <stdio.h>
#include <stdlib.h>

int string_length(const char *str);
char *reverse(const char *str);

int  main(int argc, char *argv[]){

    const char * a = "";
    const char * b = "Tonsley";
    const char * c = "Flinders University";

    char * aR = reverse(a);
    char * bR = reverse(b);
    char * cR = reverse(c);

    printf("\'%s\'\n",aR);
    printf("\'%s\'\n",bR);
    printf("\'%s\'\n",cR);

    free(aR);
    free(bR);
    free(cR);

    return 0;
}

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