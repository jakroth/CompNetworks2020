#include <stdio.h>
#include <unistd.h> 

int main(int argc, char * argv[]){

    int ch;
    
    while((ch = getchar()) != EOF){
        if(ch == ' '){
            fflush(stdout);
            usleep(200000);
        }
        putchar(ch);
    }
}
