#include <conio.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;
 
#define ERROR_FILE_OPEN -3
 
int main(int argc, char *argv[]) {
    if(argc!=2)
    {
        cout<<"Wrong format. Correct : GenerateTape 'integer' "<<endl;
        return 0;
    }
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