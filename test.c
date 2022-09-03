#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define FILENAME "jokes.txt"

typedef struct
{
    int number;
    int length;
} fileSizes;

void openFile(FILE** file, char* name)
{
    *file = fopen(name, "r");

    if (*file == NULL)
        printf("file can't be opened \n");
}

fileSizes fileSizeOf(FILE** file)
{
    int lines = 0;            // number of arrays
    int max_line_length = 0;  // length of arrays

    int curr_line_length = 0; // helper value
    char ch;                  // current char
    while (!feof(*file))
    {
        ch = fgetc(*file);
        if (ch != '\n')
            curr_line_length++;
        else
        {
            lines++;
            if (curr_line_length > max_line_length)
                max_line_length = curr_line_length;
            curr_line_length = 0;
        }
    }

    return (fileSizes){.number = lines, .length = max_line_length};
}

char** loadLines(FILE** file, int num, int length)
{
    char** lines = malloc(sizeof(char*) * num);

    int i = 0;
    while (!feof(*file))
    {
        lines[i] = malloc(length);
        fgets(lines[i], length, *file);
        i++;
    }
    return lines;
}

char* randomLine(char** lines, int num)
{
    int r = rand() % num;
    return lines[r];
}

int main()
{
    srand(time(NULL));
    FILE* ptr;
    openFile(&ptr, FILENAME);

    fileSizes sizes = fileSizeOf(&ptr);
    printf("Read %d lines\n", sizes.number);
    printf("Longest line %d\n", sizes.length);
    rewind(ptr);

    sizes.length++;
    char** jokes = loadLines(&ptr, sizes.number, sizes.length);

    for(int q = 0; q<100; q++)
    {
        char* line = randomLine(jokes, sizes.number);
        printf("%s\n", line);
    }


    // cleanup
    fclose(ptr);
    free(jokes);
    return 0;
}
