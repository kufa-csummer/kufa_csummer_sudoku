/*
 * sudoku.h - Common interface for the Sudoku game project.
 *
 * This is a collaborative project split among THREE developers. Each developer
 * implements the functions in their assigned section below. All developers share
 * the same data structures defined here, so read them carefully.
 *
 *   Developer 1 (LOGIC):   Sudoku puzzle generation, validation, and solving.
 *   Developer 2 (RENDER):  Everything drawn on screen with raylib.
 *   Developer 3 (INPUT):   Player input, cell selection, and game flow control.
 *
 * Rules of the road:
 *   - Do NOT modify this header unless all three developers agree.
 *   - Do NOT implement another developer's functions.
 *   - Each .c file should #include "sudoku.h" and raylib.h.
 *   - Use the provided types; do not introduce globals if you can avoid it.
 */

#ifndef SUDOKU_H
#define SUDOKU_H

#include <stdbool.h>
#include "raylib.h"

/* ------------------------------------------------------------------ */
/* Constants                                                          */
/* ------------------------------------------------------------------ */

#define BOARD_SIZE      9          /* 9x9 sudoku grid                  */
#define BOX_SIZE        3          /* 3x3 sub-boxes                    */
#define CELL_PIXELS     56         /* on-screen size of one cell (px)  */
#define BOARD_PIXELS    (BOARD_SIZE * CELL_PIXELS)
#define BOARD_OFFSET_X  40         /* top-left x of the board (px)     */
#define BOARD_OFFSET_Y  80         /* top-left y of the board (px)     */
#define MAX_MISTAKES    3          /* wrong moves before game over     */

/* ------------------------------------------------------------------ */
/* Shared types                                                       */
/* ------------------------------------------------------------------ */

/* Overall state of the game, driven by Developer 3. */
typedef enum {
    GAME_MENU,         /* title screen                          */
    GAME_PLAYING,      /* active puzzle                         */
    GAME_PAUSED,       /* paused overlay                        */
    GAME_WIN,          /* puzzle solved                         */
    GAME_OVER          /* too many mistakes                     */
} GameState;

/* Puzzle difficulty (used by Developer 1 when generating). */
typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD
} Difficulty;

// note (mojtaba): I am not feeling good about having a lot of booleans in the struct, maybe thinking of another encoding
// note (mojtaba): Also, this isError does not seem useful, given that we are already tracking the number of mistakes?
/* A single cell of the board. */
typedef struct {
    int  value;            /* 0 = empty, 1..9 = the digit          */
    bool isGiven;          /* true if part of the original puzzle  */
    bool notes[BOARD_SIZE];/* pencil marks, notes[i] for digit i+1 */
    bool isError;          /* true if the entered value is wrong   */
} Cell;

/* The whole game. Passed by pointer to every function below. */
typedef struct {
    Cell        grid[BOARD_SIZE][BOARD_SIZE];
    int         selectedRow;   /* currently selected row, -1 = none */
    int         selectedCol;   /* currently selected col, -1 = none */
    GameState   state;
    Difficulty  difficulty;
    int         mistakes;
    double      startTime;     /* seconds since epoch at game start  */
    double      elapsedTime;   /* accumulated play time, in seconds  */
    bool        noteMode;      /* true = number keys set notes       */
} Game;

/* ================================================================== */
/* DEVELOPER 1 - LOGIC                                                  */
/* Puzzle generation, validation, solving, and win detection.         */
/* Implement these in: logic.c                                        */
/* ================================================================== */

/*
 * Reset the board to a fresh, empty state and pick a new puzzle of the
 * given difficulty. Fills in grid, sets isGiven for clue cells, and
 * leaves player cells empty (value 0).
 */
void game_generatePuzzle(Game *game, Difficulty difficulty);

/*
 * Return true if placing `num` (1..9) at (row, col) is legal according to
 * standard sudoku rules: no duplicate in the row, column, or 3x3 box.
 * Ignores the current value at (row, col). Returns false for num == 0.
 */
bool logic_isValidMove(const Cell grid[BOARD_SIZE][BOARD_SIZE],
                       int row, int col, int num);

/*
 * Place `num` into the selected cell. If the cell is a given clue, do
 * nothing. Otherwise set the value and mark isError when num conflicts
 * with the solution (Developer 1 must keep the solution internally). When
 * isError becomes true, increment game->mistakes.
 */
void logic_placeNumber(Game *game, int row, int col, int num);

/*
 * Toggle the pencil-mark note for `num` in the cell at (row, col).
 * Does nothing for given cells.
 */
void logic_toggleNote(Game *game, int row, int col, int num);

/*
 * Clear the value and notes of the cell at (row, col) if it is not a given.
 */
void logic_clearCell(Game *game, int row, int col);

/*
 * Return true if every cell is filled with a non-error value (puzzle solved).
 */
bool logic_checkWin(const Game *game);

/*
 * (Optional helper) Solve the given grid in place using backtracking.
 * Returns true if a solution exists. Useful for testing; not required by
 * the GUI, but Developer 1 may use it internally during generation.
 */
bool logic_solveSudoku(Cell grid[BOARD_SIZE][BOARD_SIZE]);

/* ================================================================== */
/* DEVELOPER 2 - RENDER                                                 */
/* Everything drawn on screen with raylib.                            */
/* Implement these in: render.c                                       */
/* ================================================================== */

/*
 * Draw the whole board: background, grid lines, digits, notes, selection
 * highlight, and error highlights. Called every frame while playing.
 */
void render_drawBoard(const Game *game);

/*
 * Draw the 9x9 grid lines, with thicker lines separating the 3x3 boxes.
 */
void render_drawGridLines(void);

/*
 * Draw the contents (value / notes / highlights) of a single cell.
 */
void render_drawCell(const Game *game, int row, int col);

/*
 * Draw the heads-up display: timer, mistake count, difficulty, and any
 * on-screen hints (such as "Note mode: ON").
 */
void render_drawHUD(const Game *game);

/*
 * Draw the title menu showing difficulty choices and a Start prompt.
 * Returns the rectangle of each menu button through the `buttons` array
 * (up to 3 entries) so Developer 3 can do hit-testing. Returns the count.
 */
int  render_drawMenu(Rectangle buttons[3]);

/*
 * Draw the win overlay (congratulations + time + restart prompt).
 */
void render_drawWinScreen(const Game *game);

/*
 * Draw the game-over overlay (too many mistakes + restart prompt).
 */
void render_drawGameOverScreen(const Game *game);

/*
 * Draw the pause overlay.
 */
void render_drawPauseScreen(void);

/* ================================================================== */
/* DEVELOPER 3 - INPUT & GAME FLOW                                      */
/* Mouse/keyboard handling, cell selection, state transitions.        */
/* Implement these in: input.c                                        */
/* ================================================================== */

/*
 * Called every frame. Reads mouse and keyboard input and dispatches to the
 * other input helpers below based on game->state.
 */
void input_handle(Game *game);

/*
 * Convert a mouse position to board coordinates. Returns true and fills
 * *row/*col (0..8) if the position is over a cell; returns false otherwise.
 */
bool input_screenToCell(Vector2 mouse, int *row, int *col);

/*
 * Convert board coordinates to the on-screen center point of that cell.
 */
Vector2 input_cellToScreen(int row, int col);

/*
 * Set the selected cell. Use row/col of -1 to clear the selection.
 */
void input_selectCell(Game *game, int row, int col);

/*
 * Handle a digit key press (1..9): in note mode toggle a note, otherwise
 * place the number via logic_placeNumber.
 */
void input_handleNumber(Game *game, int num);

/*
 * Handle non-digit keys: arrows to move selection, backspace/delete to
 * clear, space to toggle note mode, P to pause, R to restart, etc.
 */
void input_handleKeys(Game *game);

/*
 * Handle mouse clicks depending on the current state: pick a difficulty on
 * the menu, select a cell while playing, click restart on win/game-over.
 */
void input_handleMouse(Game *game);

/*
 * Start a new game: reset the board via Developer 1, reset the timer and
 * mistake count, and switch state to GAME_PLAYING.
 */
void game_startNew(Game *game, Difficulty difficulty);

/*
 * Reset everything to the menu state.
 */
void game_reset(Game *game);

/*
 * Advance the game one frame: update the elapsed time and check for a win.
 */
void game_update(Game *game);

/* ================================================================== */
/* main.c - Provided for you                                          */
/* You normally do not need to edit this; it wires everything together */
/* with the raylib main loop.                                         */
/* ================================================================== */

#endif /* SUDOKU_H */
