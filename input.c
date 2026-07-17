#include "sudoku.h"
#include "raylib.h"

/*========== INPUT & GAME FLOW ==========*/

void input_handle(Game *game) {
    if (!game) return;

    switch (game->state) {
        case GAME_MENU:
            input_handleMouse(game);
            break;

        case GAME_PLAYING:
            input_handleMouse(game);
            input_handleKeys(game);
            break;

        case GAME_PAUSED:
            if (IsKeyPressed(KEY_P)) {
                game->state = GAME_PLAYING;
            }
            break;

        case GAME_WIN:
        case GAME_OVER:
            input_handleMouse(game);
            if (IsKeyPressed(KEY_R)) {
                game_reset(game);
            }
            break;
    }
}

bool input_screenToCell(Vector2 mouse, int *row, int *col) {
    float relativeX = mouse.x - BOARD_OFFSET_X;
    float relativeY = mouse.y - BOARD_OFFSET_Y;

    if (relativeX < 0 || relativeX >= BOARD_PIXELS || 
        relativeY < 0 || relativeY >= BOARD_PIXELS) {
        return false;
    }

    *col = (int)(relativeX / CELL_PIXELS);
    *row = (int)(relativeY / CELL_PIXELS);

    return true;
}

Vector2 input_cellToScreen(int row, int col) {
    Vector2 center;
    center.x = BOARD_OFFSET_X + (col * CELL_PIXELS) + (CELL_PIXELS / 2.0f);
    center.y = BOARD_OFFSET_Y + (row * CELL_PIXELS) + (CELL_PIXELS / 2.0f);
    return center;
}

void input_selectCell(Game *game, int row, int col) {
    if (!game) return;
    
    if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE) {
        game->selectedRow = row;
        game->selectedCol = col;
    } else {
        game->selectedRow = -1;
        game->selectedCol = -1;
    }
}

void input_handleNumber(Game *game, int num) {
    if (!game) return;

    int r = game->selectedRow;
    int c = game->selectedCol;
    if (r == -1 || c == -1) return;

    if (game->noteMode) {
        logic_toggleNote(game, r, c, num);
    } else {
        logic_placeNumber(game, r, c, num);
        
        if (game->mistakes >= MAX_MISTAKES) {
            game->state = GAME_OVER;
        }
    }
}

void input_handleKeys(Game *game) {
    if (!game) return;

    if (game->selectedRow != -1 && game->selectedCol != -1) {
        if (IsKeyPressed(KEY_UP)) {
            game->selectedRow = (game->selectedRow - 1 + BOARD_SIZE) % BOARD_SIZE;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            game->selectedRow = (game->selectedRow + 1) % BOARD_SIZE;
        }
        if (IsKeyPressed(KEY_LEFT)) {
            game->selectedCol = (game->selectedCol - 1 + BOARD_SIZE) % BOARD_SIZE;
        }
        if (IsKeyPressed(KEY_RIGHT)) {
            game->selectedCol = (game->selectedCol + 1) % BOARD_SIZE;
        }
    } else {
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN) || 
            IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) {
            input_selectCell(game, 0, 0);
        }
    }

    if (IsKeyPressed(KEY_SPACE)) {
        game->noteMode = !game->noteMode;
    }

    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_DELETE)) {
        if (game->selectedRow != -1 && game->selectedCol != -1) {
            logic_clearCell(game, game->selectedRow, game->selectedCol);
        }
    }

    if (IsKeyPressed(KEY_P)) {
        game->state = GAME_PAUSED;
    }

    if (IsKeyPressed(KEY_R)) {
        game_reset(game);
    }

    for (int key = KEY_ONE; key <= KEY_NINE; key++) {
        if (IsKeyPressed(key)) {
            int digitValue = key - KEY_ONE + 1; 
            input_handleNumber(game, digitValue);
        }
    }
}

void input_handleMouse(Game *game) {
    if (!game) return;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();

        if (game->state == GAME_MENU) {
            Rectangle menuButtons[3];
            int numButtons = render_drawMenu(menuButtons);

            for (int i = 0; i < numButtons; i++) {
                if (CheckCollisionPointRec(mousePos, menuButtons[i])) {
                    game_startNew(game, (Difficulty)i);
                    break;
                }
            }
        } 
        else if (game->state == GAME_PLAYING) {
            int clickedRow, clickedCol;
            if (input_screenToCell(mousePos, &clickedRow, &clickedCol)) {
                input_selectCell(game, clickedRow, clickedCol);
            } else {
                input_selectCell(game, -1, -1);
            }
        } 
        else if (game->state == GAME_WIN || game->state == GAME_OVER) {
            game_reset(game);
        }
    }
}

void game_startNew(Game *game, Difficulty difficulty) {
    if (!game) return;

    game_generatePuzzle(game, difficulty);

    game->selectedRow = -1;
    game->selectedCol = -1;
    game->mistakes = 0;
    game->noteMode = false;
    game->difficulty = difficulty;

    game->elapsedTime = 0.0;
    game->startTime = GetTime();

    game->state = GAME_PLAYING;
}

void game_reset(Game *game) {
    if (!game) return;
    
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            game->grid[r][c].value = 0;
            game->grid[r][c].isGiven = false;
            game->grid[r][c].isError = false;
            for (int n = 0; n < BOARD_SIZE; n++) {
                game->grid[r][c].notes[n] = false;
            }
        }
    }
    game->state = GAME_MENU;
}

void game_update(Game *game) {
    if (!game) return;

    if (game->state == GAME_PLAYING) {
        game->elapsedTime = GetTime() - game->startTime;

        if (logic_checkWin(game)) {
            game->state = GAME_WIN;
        }
    }
}