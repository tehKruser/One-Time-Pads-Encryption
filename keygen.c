#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


int GetRandomNum(int min, int max){
    int range = max - min + 1;
    int randNum = rand() % range + min;
    return randNum;
}

int main( int argc, char *argv[] ) {
    srand(time(NULL));

    int key_length = atoi(argv[1]) + 1;

    int i;
    for(i = 0; (argv[1])[i] != 0; i++){
        if(!isdigit((argv[1])[i])){
            fprintf(stderr, "error: arg[1] must be an integer\n");
            return 1;
        }
    }

    char key[key_length];
    memset(key, '\0', key_length);

    for(i = 0; i < key_length - 1; i++){
        int j = GetRandomNum(1, 27);
        if(j == 27){
            key[i] = ' ';
        } else {
            key[i] = j + 64;
        }
    }

    key[key_length - 1] = '\n';

    printf("%s", key);

    return 0;
}
