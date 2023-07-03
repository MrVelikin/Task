#include <conio.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ERROR_FILE_OPEN -3

using namespace std;
 
int main(int argc, char *argv[]) 
{
    fstream Conv;
    Conv.open(argv[2],ios::out);

    FILE *put = NULL;
    int number=0;
    put = fopen(argv[1], "rb");
    if (put == NULL) {
        printf("Error opening file");
        getch();
        exit(ERROR_FILE_OPEN);
    }
    while (fread(&number, sizeof(number), 1, put) == 1) 
    {
        Conv<<number<<" ";
    }
    Conv.close();
    fclose(put);
    return 0;
}