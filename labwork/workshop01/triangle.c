#include <stdio.h>

int main(int argc, const char *argv[]) {

    for(int i = 0; i < 5; ++i){
        for(int j = 0;j<=i;++j){
            printf("*");
        }
        putchar('\n');
    }
    return 0;
}
