#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ERROR_FILE_OPEN -3
 
int main(int argc, char *argv[]) {
    FILE *output = NULL;
    int number=0;
    srand(time(NULL));
    output = fopen("test.bin", "wb");
    if (output == NULL) {
        printf("Error opening file");
        getch();
        exit(ERROR_FILE_OPEN);
    }
    for(int i=0;i<atoi(argv[1]);i++)
    { 
        number=rand();
        fwrite(&number, sizeof(int), 1, output);
    }
    fclose(output);
    return 0;
}