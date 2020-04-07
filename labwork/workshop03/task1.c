#include <stdio.h>
#include <limits.h>

int main(int argc, char * argv[]){

    printf("The number of bits in a byte: %d\n", CHAR_BIT);
    printf("size of char:     %lu byte(s) (%lu bits)\n", sizeof(char), sizeof(char)*CHAR_BIT);
    printf("size of short:    %lu byte(s) (%lu bits)\n", sizeof(short), sizeof(short)*CHAR_BIT);
    printf("size of int:      %lu byte(s) (%lu bits)\n", sizeof(int), sizeof(int)*CHAR_BIT);
    printf("size of long:     %lu byte(s) (%lu bits)\n", sizeof(long), sizeof(long)*CHAR_BIT);
    printf("size of float:    %lu byte(s) (%lu bits)\n", sizeof(float), sizeof(float)*CHAR_BIT);
    printf("size of double:   %lu byte(s) (%lu bits)\n", sizeof(double), sizeof(double)*CHAR_BIT);
    printf("size of char *:   %lu byte(s) (%lu bits)\n", sizeof(char *), sizeof(char *)*CHAR_BIT);    
    printf("size of int *:    %lu byte(s) (%lu bits)\n", sizeof(int *), sizeof(int *)*CHAR_BIT);      
    printf("size of double *: %lu byte(s) (%lu bits)\n", sizeof(double *), sizeof(double *)*CHAR_BIT);       
    printf("\n");

    double darr[7];
    char flo [] = "Flinders University Online";
    printf("size of double darr[7]: %lu byte(s) (%lu bits)\n", sizeof(darr), sizeof(darr)*CHAR_BIT);
    printf("size of \"Flinders University Online\": %lu byte(s) (%lu bits)\n", sizeof(flo), sizeof(flo)*CHAR_BIT); 
    printf("\n");

    printf("length of double darr[7]: %lu\n", sizeof(darr)/sizeof(double));
    printf("length of \"Flinders University Online\": %lu (including the \'\\0\' character the end)\n", sizeof(flo)/sizeof(char));

    return 0; 
}

// The pointers are all the same size, at 64bits. This is because I am on a 64-bit system (I have the 64-bit VM installed). 

// The size of double darr[7] will be 7 * 8 bytes = 56 bytes (448 bits).
// The size of "Flinders University Online" is the same as char str[27], because declared inside "" it includes '\0' at the end.
// So it will be 27 * 1 byte = 27 bytes (216 bits).

// You could determine the length of an array by using sizeof(arr)/sizeof(type), where type is the type of element in the array.