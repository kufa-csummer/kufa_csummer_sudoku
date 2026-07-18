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
        game->mistakes++;
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



// special function to check for unique solutions
void countSolutions(Cell grid[BOARD_SIZE][BOARD_SIZE], int *count) {
    if (*count > 1) return; 

    int row = -1;
    int col = -1;
    bool isEmpty = false;

    // Finding the next empty cell
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (grid[r][c].value == 0) {
                row = r;
                col = c;
                isEmpty = true;
                break;
            }
        }
        if (isEmpty) break;
    }

    // If there are no empty cells, we found a complete valid solution
    if (!isEmpty) {
        (*count)++;
        return;
    }

    // try all the possible combinations of numbers
    for (int num = 1; num <= 9; num++) {
        if (logic_isValidMove(grid, row, col, num)) {
            grid[row][col].value = num;
            countSolutions(grid, count);
            grid[row][col].value = 0; // Go back (backtracking)
        }
    }
}



void game_generatePuzzle(Game *game, Difficulty difficulty) {
    int targetHints = 0;
    int currentHints = 81;
    int array[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    // Clear the grid before generating a new puzzle
    logic_clearGrid(game->grid);

    // Shuffle the array using rand() function and modulus operator
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

    // Solve to generate a complete, valid Sudoku board
    logic_solveSudoku(game->grid);

    // Save the results into the solutionGrid for later use
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            solutionGrid[r][c] = game->grid[r][c].value;
            game->grid[r][c].isGiven = true; 
        }
    }

    if (difficulty == DIFFICULTY_EASY) targetHints = 40;
    if (difficulty == DIFFICULTY_MEDIUM) targetHints = 30;
    if (difficulty == DIFFICULTY_HARD) targetHints = 20;

    /* 
    * The problem was that the generation algorithm randomly removed clues without checking, 
    * which often resulted in a puzzle with multiple valid solutions. Because the game strictly 
    * checked inputs against a single hidden answer, players were getting a "mistake" 
    * for placing logically valid numbers that belonged to an alternative solution. 
    * 
    *  [sorry for the long explanation, but I wanted to make sure you understood the issue]
    * 
    * Therefore, we need to ensure that the puzzle has exactly one unique solution. We do this 
    * by checking the total number of solutions after each removal of a number from the grid. 
    * If the number of solutions is greater than one, we put the number back and try removing 
    * a different cell to maintain a mathematically perfect board.
    */

    int failedAttempts = 0;
    int maxFailedAttempts = 100; // fail attempts to avoid infinite loops in unsolvable puzzles

    while (currentHints > targetHints && failedAttempts < maxFailedAttempts) {
        int random_row = rand() % BOARD_SIZE;
        int random_column = rand() % BOARD_SIZE;

        if (game->grid[random_row][random_column].isGiven) {
            // Temporarily erase the cell
            int backupValue = game->grid[random_row][random_column].value;
            game->grid[random_row][random_column].isGiven = false;
            game->grid[random_row][random_column].value = 0;

            // Count solutions to ensure the puzzle is still unique
            int solutions = 0;
            countSolutions(game->grid, &solutions);

            if (solutions == 1) {
                // The puzzle is still unique so keep the cell erased
                currentHints--;
                failedAttempts = 0; 
            } else {
                // we still got Multiple solutions created. Therefore, Put the number back
                game->grid[random_row][random_column].isGiven = true;
                game->grid[random_row][random_column].value = backupValue;
                failedAttempts++;
            }
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
