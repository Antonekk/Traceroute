#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "helpers.c"


#define eprintf(...) fprintf (stderr, __VA_ARGS__)


int main(int argc, char* argv[]){

    if (argc != 2){
        eprintf("Invalid argument count\n");
        return EXIT_FAILURE;
    }

    if(!checkValidIP(argv[1])){
        eprintf("Invalid ip adress\n");
        return EXIT_FAILURE;
    }


    


}