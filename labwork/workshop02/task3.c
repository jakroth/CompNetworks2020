#include <stdio.h>
#include <unistd.h> 
#include <ctype.h>

int main(int argc, char * argv[]){

    int ch;
    
    while((ch = getchar()) != EOF){
        if(ch == ' '){
            fflush(stdout);
            usleep(200000);
            putchar('\n');
        }
        if(isalpha(ch)){
            putchar(ch);
        }
    }
    putchar('\n');
    return 0;
}
