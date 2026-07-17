#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "sudoku.h"

static int solutionGrid[BOARD_SIZE][BOARD_SIZE];
#define BOX_SIZE 3 


bool logic_isValidMove(const Cell grid[BOARD_SIZE][BOARD_SIZE], int row, int col, int num) {
    if (num == 0) {
        return false;
    }


    // checks for duplicates in the same row and column
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (i != col && grid[row][i].value == num) {
            return false;
        }
        if (i != row && grid[i][col].value == num) {
            return false;
        }
    }

    // starting row number of the current box
    int startRow = (row / BOX_SIZE) * 3;
    // starting column number of the current box
    int startCol = (col / BOX_SIZE) * 3;
    
    for (int boxRow = 0; boxRow < 3; boxRow++) {
        for (int boxCol = 0; boxCol < 3; boxCol++) {
            // loop through the cells in the current box
            int cellRow = startRow + boxRow;
            int cellCol = startCol + boxCol;
            
            // skip the cell being checked
            if (cellRow != row || cellCol != col) {
                if (grid[cellRow][cellCol].value == num) {
                    return false;
                }
            }
        }
    }
    // return 1 if the number is valid in the given cell
    return true;
}



bool logic_solveSudoku(Cell grid[BOARD_SIZE][BOARD_SIZE]) {
    int row = -1;
    int col = -1;
    bool isEmpty = false;

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (grid[r][c].value == 0) {
                row = r;
                col = c;
                isEmpty = true;
                break;
            }
        }
        if (isEmpty) {
            break; 
        }
    }
    if (!isEmpty) {
        return true;
    }

    for (int num = 1; num <= 9; num++) {
        
        if (logic_isValidMove(grid, row, col, num)) {

            grid[row][col].value = num;

            if (logic_solveSudoku(grid)) {

                return true; 
            }

            grid[row][col].value = 0;
        }
    }
    return false;
}







void logic_placeNumber(Game *game, int row, int col, int num) {
    if (game->grid[row][col].isGiven) {
        return;
    }

    if (!logic_isValidMove(game->grid, row, col, num)) {
        return; 
    }
    
    game->grid[row][col].value = num;
    if (num != solutionGrid[row][col]) {
        game->grid[row][col].isError = true;
        game->mistakes++;
    } else {
        game->grid[row][col].isError = false;
    }
}



void logic_clearCell(Game *game, int row, int col){
    if (!game->grid[row][col].isGiven) {
        game->grid[row][col].value = 0;
        game->grid[row][col].isError = false;
        for (int i = 0; i < 9; i++) {
            game->grid[row][col].notes[i] = false;
        }
    }
}



bool logic_checkWin(const Game *game){ 
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if (game->grid[row][col].value == 0 || game->grid[row][col].isError) {
                return false;
            }
        }
    }
    return true;
}




void logic_clearGrid(Cell grid[BOARD_SIZE][BOARD_SIZE]) {
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            grid[row][col].value = 0;
            grid[row][col].isGiven = false;
            grid[row][col].isError = false;
            for (int i = 0; i < BOARD_SIZE; i++) {
                grid[row][col].notes[i] = false;
            }
        }
    }

}



void game_generatePuzzle(Game *game, Difficulty difficulty) {
    int targetHints=0;
    int currentHints=81;
    int array[9]={1,2,3,4,5,6,7,8,9};
    // Clear the grid before generating a new puzzle
    logic_clearGrid(game->grid);

    // shuffle the array using rand() function and modulus operator
    for (int i = BOARD_SIZE - 1; i > 0; i--) {
    int j = rand() % (i + 1); 
    int temp = array[i];
    array[i] = array[j];
    array[j] = temp;
}

    // Assign the shuffled values to the first row of the grid
    for (int c = 0; c < BOARD_SIZE; c++) {
        game->grid[0][c].value = array[c];
    }

    logic_solveSudoku(game->grid);

    // save the results into the solutionGrid for later use
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            solutionGrid[r][c] = game->grid[r][c].value;
            game->grid[r][c].isGiven = true; 
        }
    }


    if (difficulty == DIFFICULTY_EASY) targetHints = 40;
    if (difficulty == DIFFICULTY_MEDIUM) targetHints = 30;
    if (difficulty == DIFFICULTY_HARD) targetHints = 20;

    while (currentHints > targetHints){
        int random_row = rand() % BOARD_SIZE;
        int random_column = rand() % BOARD_SIZE;

        if (game->grid[random_row][random_column].isGiven){
            game->grid[random_row][random_column].isGiven = false;
            game->grid[random_row][random_column].value = 0;
            currentHints--;
        }
    }
    
}

    


void logic_toggleNote(Game *game, int row, int col, int num) {
    // ignore given cells and invalid numbers
    if (game->grid[row][col].isGiven) {
        return;
    }

    if (num >= 1 && num <= BOARD_SIZE) {

        int noteIndex = num - 1;
        

        game->grid[row][col].notes[noteIndex] = !game->grid[row][col].notes[noteIndex];
    }
}
