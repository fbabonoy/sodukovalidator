#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define NUM_CHILD_THREADS 27

FILE *outputFile;

// global variables
int sudokuArray[9][9];

bool rowValidator[9];
bool colValidator[9];
bool subGridValidator[9];

typedef struct {
    int topRow;
    int bottomRow;
    int leftColumn;
    int rightColumn;
}
index;

// threads for checking
void *col_runner(void *param) {
    pthread_t self;
    index *params = (index *)param;
    int lc = params->leftColumn;
    bool isValid[9] = {0};
    int i;
    for (i = 0; i < 9; i++) {
        int value = sudokuArray[i][lc];
        if (value < 1 || value > 9 || isValid[value - 1] == 1) {
            self = pthread_self();
            printf("Column: %lx invalid\n", (unsigned long)self);
            fprintf(outputFile, "Column: %lx invalid\n", (unsigned long)self);

            pthread_exit(0);
        }
        else {
            isValid[value - 1] = 1;
        }
    }


    self = pthread_self();
    colValidator[lc] = 1;
    subGridValidator[lc] = 1;
    printf("Column: %lx valid\n", (unsigned long)self);
    fprintf(outputFile, "Column: %lx valid\n", (unsigned long)self);
    pthread_exit(0);
}
void *row_runner(void *param) {
    pthread_t self;
    index *params = (index *)param;
    int tr = params-> topRow;
    bool isValid[9] = {0};
    int i;
    for (i = 0; i < 9; i++) {
        int value = sudokuArray[tr][i];
        if (value < 1 || value > 9 || isValid[value - 1] == 1) {
            self = pthread_self();
            printf("Row: %lx invalid\n", (unsigned long)self);
            fprintf(outputFile, "Row: %lx invalid\n", (unsigned long)self);

            pthread_exit(0);
        }
        else {
            isValid[value - 1] = 1;
        }
    }
    self = pthread_self();
    rowValidator[tr] = 1;
    printf("Row: %lx valid\n", (unsigned long)self);
    fprintf(outputFile, "Row: %lx valid\n", (unsigned long)self);

    pthread_exit(0);
}

// void *sub_grid_runner(void *param);
void *subGrid_Array(void *param) {
    pthread_t self;
    index *params = (index *)param;

    int tr = params->topRow;
    int br = params->bottomRow;
    int lc = params->leftColumn;
    int rc = params->rightColumn;

    // printf("%d , %d, %d , %d", tr, br, lc, rc);
    bool isValid[9] = {0};
    int r;
    for (r = tr; r <= br; r++) {

        int i;

        for (i = lc; i <= rc; i++) {
            int value = sudokuArray[r][i];
            if (value < 1 || value > 9 || isValid[value - 1] == 1) {
                subGridValidator[lc] = 0;
                self = pthread_self();
                printf("Subgrid: %lx invalid\n", (unsigned long)self);
                fprintf(outputFile, "Subgrid: %lx invalid\n", (unsigned long)self);

                pthread_exit(0);
            }
            else {
                isValid[value - 1] = 1;
            }
        }
    }

    self = pthread_self();
    printf("Subgrid: %lx valid\n", (unsigned long)self);
    fprintf(outputFile, "Subgrid: %lx valid\n", (unsigned long)self);

    pthread_exit(0);
}

int main() {
    FILE *inputFile;

    inputFile = fopen("SudokuPuzzle.txt", "r");

    outputFile = fopen("testResults.txt", "w");

    pthread_t thread_id[NUM_CHILD_THREADS];

    int row, col;

    for (row = 0; row < 9; row++) {
        for(col = 0; col < 9; col++) {
            fscanf(inputFile, "%d ", &sudokuArray[row][col]);
            printf("%d  ", sudokuArray[row][col]);
            fprintf(outputFile, "%d  ", sudokuArray[row][col]);
        }
        printf("\n");
        fprintf(outputFile, "\n");
    }

    printf("\n\n");
    fprintf(outputFile, "\n");
    fclose(inputFile);

    index colArray[9];
    index rowArray[9];
    index subGridArray[9];

    // create col threads
    int i;
    for (i = 0; i < 9; i++) {
        // index *columnData = (index *)malloc(sizeof(index));
        colArray[i].topRow = 0;
        colArray[i].bottomRow = 8;
        colArray[i].leftColumn = i;
        colArray[i].rightColumn = i;
        pthread_create(&(thread_id[i]), NULL, col_runner, &(colArray[i]));
    }

    // create row threads
    for (i = 0; i < 9; i++) {
        // index *rowData = (index *)malloc(sizeof(index));
        rowArray[i].topRow = i;
        rowArray[i].bottomRow = i;
        rowArray[i].leftColumn = 0;
        rowArray[i].rightColumn = 8;
        pthread_create(&(thread_id[i + 9]), NULL, row_runner, &(rowArray[i]));
    }

    // create 3d threads
    int top, bottom;
    int k = 0;

    for (i = 0; i < 3; i++) {
        // index *columnData = (index *)malloc(sizeof(index));

        top = i * 3;
        bottom = top + 2;
        int i;

        for (i = 0; i < 3; i++){

            // printf("\n%d , %d , %d , %d\n\n", top,bottom, i * 3, (i * 3) + 2 );
            subGridArray[k].topRow = top;
            subGridArray[k].bottomRow = bottom;
            subGridArray[k].leftColumn = i * 3;
            subGridArray[k].rightColumn = (i * 3) + 2;
            pthread_create(&(thread_id[k + 18]), NULL, subGrid_Array, &(subGridArray[k]));
            k++;

        }
    }

    // join threads
    for(i = 0; i < 27; i++) {
        pthread_join(thread_id[i], NULL);
    }


    for(i = 8; i < 9; i++) {
        if(colValidator[i] == 0) {
            printf("COLUMNS INVALID\n");
            return EXIT_SUCCESS;
        }
        else if (rowValidator[i] == 0) {
            printf("ROW INVALID\n");
            return EXIT_SUCCESS;
        }
        else if(subGridValidator[i] == 0) {
            printf("sub grid INVALID\n");
            return EXIT_SUCCESS;
        }
    }

    printf("\nColumn: valid\nRow: valid\nSubgrid: valid\nSudoku puzzle: valid\n");
    fprintf(outputFile, "\nColumn: valid\nRow: valid\nSubgrid: valid\nSudoku puzzle: valid\n");

    fclose(outputFile);

    return EXIT_SUCCESS;
}

