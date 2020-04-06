#include <stdio.h>
#include <unistd.h> 
#include <ctype.h>
#include <string.h>

int main(int argc, char * argv[]){

    int ch = 0;
    int lines = 0;
    int characters = 0;
    int words = 0;
    int upperChars = 0;
    int lowerChars = 0;
    int vowels = 0;
    
    while((ch = getchar()) != EOF){
        characters++;
        if(ch == '\n') lines++;
        if(isspace(ch)) words++;
        if(isupper(ch)) upperChars++;
        if(islower(ch)) lowerChars++;
        if(strchr("aeiouAEIOU", ch) != NULL) vowels++;
    }

    printf("%3d lines \n", lines);
    printf("%3d words \n", words);
    printf("%3d characters \n", characters);
    printf("%3d vowels \n", vowels);
    printf("%3d lowercase \n", lowerChars);
    printf("%3d uppercase \n", upperChars);

    return 0;
}
