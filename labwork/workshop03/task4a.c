#include <stdio.h>

int string_length(const char * str);

int  main(int argc, char * argv[]){

    const char * a = "";
    const char * b = "Tonsley";
    const char * c = "Flinders University";

    printf("%d characters in \'%s\'\n",string_length(a),a);
    printf("%d characters in \'%s\'\n",string_length(b),b);
    printf("%d characters in \'%s\'\n",string_length(c),c);

    return 0;
}

int string_length(const char * str){
    int i = 0;
    while(str[i]) i++;
    return i;
}

//if the string were not properly null terminated, my string_length function would continue through the memory space until it found a 0. 
//the length of the string literal "Operating\0System" would be 9, cause the \0 would terminate my function. 