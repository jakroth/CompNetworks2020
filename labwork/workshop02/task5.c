#include <stdio.h>

static const int max_word_length = 50;
void copy(char src[], char dst[], int count);
void copy2(char src[], char dst[], int count);
void copy3(char src[], char dst[], int count);

int main(int argc, char * argv[]){

    int c = 0;
    int i = 0;
    char curr_word[max_word_length];
    char longest_word[max_word_length];
    int longest = 0;
    int char_count = 0;
    
    while((c = getc(stdin)) != EOF && i < max_word_length-1){
        curr_word[char_count] = c;
        if(c == '\n'){
            curr_word[char_count] = '\0';
            if(char_count > longest){
                copy(curr_word,longest_word,char_count;
                longest = char_count;
            }
            char_count = 0;
        }
        else{
            char_count++;
        }
    }
    printf("Longest word: %s\n", longest_word);
    printf("Length: %d\n", longest);
    return 0;
}

//copy function using pointers, increment operators and implicit while not(\0) condition
void copy(char src[], char dst[], int count){
    while(*src){
        *dst++ = *src++;
        count--;
        if(count==0) return;
    }
}

//copy function using for loop, array syntax and returning on explicit if(\0) test
void copy2(char src[], char dst[], int count){    
    for(int i = 0; i < count; i++){
        dst[i] = src[i];
        if(src[i] == '\0') return;
    }
}

//copy function using while loop, array syntax and explicit while not(\0) condition
void copy3(char src[], char dst[], int count){    
    int i = 0;
    while(src[i] != '\0' && i < count){
        dst[i] = src[i];
        i++;
    }
}