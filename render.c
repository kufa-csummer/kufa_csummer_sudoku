#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "sudoku.h"

void render_drawGridLines(void);
void render_drawCell(const Game* game, int row, int col);

void render_drawBoard(const Game* game) {
    ClearBackground(RAYWHITE);
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            render_drawCell(game, row, col);
        }
    }
    render_drawGridLines();
}


void render_drawGridLines(void) {
    float thickness;
    int boardX = (GetScreenWidth() - BOARD_PIXELS) / 2;
    int boardY = (GetScreenHeight() - BOARD_PIXELS) / 2 + 40;
    for (int i = 0; i <= BOARD_SIZE; i++) {

        int x = boardX + i * CELL_PIXELS;
        int y = boardY + i * CELL_PIXELS;
        if (i % BOX_SIZE == 0) {
            thickness = 4.0;
        }
        else
        {
            thickness = 1.0;

        }
        DrawLineEx((Vector2) { x, boardY }, (Vector2) { x, boardY + BOARD_PIXELS }, thickness, BLACK);
        DrawLineEx((Vector2) { boardX, y }, (Vector2) { boardX + BOARD_PIXELS, y }, thickness, BLACK);
    }
}



void render_drawCell(const Game* game, int row, int col) {
    int boardX = (GetScreenWidth() - BOARD_PIXELS) / 2;
    int boardY = (GetScreenHeight() - BOARD_PIXELS) / 2 + 40;

    int x = boardX + col * CELL_PIXELS;
    int y = boardY + row * CELL_PIXELS;

    const Cell* cell = &game->grid[row][col];
    if (row == game->selectedRow && col == game->selectedCol) {
        DrawRectangle(x, y, CELL_PIXELS, CELL_PIXELS, SKYBLUE);
    }
    else
    {
        DrawRectangle(x, y, CELL_PIXELS, CELL_PIXELS, RAYWHITE);
    }
    if (cell->value != 0) {
        Color textColor;
        if (cell->isError)
        {
            textColor = RED;
        }
        else if (cell->isGiven)
        {
            textColor = DARKBLUE;
        }
        else
        {
            textColor = BLACK;
        }
        int fontsize = 30;
        int textWidth = MeasureText(TextFormat("%d", cell->value), fontsize);
        DrawText(TextFormat("%d", cell->value), x + (CELL_PIXELS - textWidth) / 2, y + (CELL_PIXELS - fontsize) / 2, fontsize, textColor);
    }
    else
    {
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (cell->notes[i])
            {
                int noteX = x + (i % BOX_SIZE) * (CELL_PIXELS / BOX_SIZE);
                int noteY = y + (i / BOX_SIZE) * (CELL_PIXELS / BOX_SIZE);
                int notesize = 12;
                int noteTextWidth = MeasureText(TextFormat("%d", i + 1), notesize);
                DrawText(TextFormat("%d", i + 1), noteX + (CELL_PIXELS / BOX_SIZE - noteTextWidth) / 2, noteY + (CELL_PIXELS / BOX_SIZE - notesize) / 2, notesize, GRAY);
            }
        }
    }
}


void render_drawHUD(const Game* game) {
    int totalSeconds = (int)game->elapsedTime;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    const char* difficultyText;
    switch (game->difficulty)
    {
    case DIFFICULTY_EASY:
        difficultyText = "Easy";
        break;
    case DIFFICULTY_MEDIUM:
        difficultyText = "Medium";
        break;
    case DIFFICULTY_HARD:
        difficultyText = "Hard";
        break;
    default:
        difficultyText = "Unknown";
        break;
    }
    int screenWidth = GetScreenWidth();

    int section1 = screenWidth / 12;
    int section2 = screenWidth * 3 / 12;
    int section3 = screenWidth * 6 / 12;
    int section4 = screenWidth * 9 / 12;

    DrawText("Time", section1, 15, 18, DARKGRAY);
    DrawText(TextFormat("%02d:%02d", minutes, seconds), section1, 40, 24, BLACK);

    DrawText("Mistakes", section2, 15, 18, DARKGRAY);
    DrawText(TextFormat("%d/%d", game->mistakes, MAX_MISTAKES), section2, 40, 24, BLACK);

    DrawText("Difficulty", section3, 15, 18, DARKGRAY);
    DrawText(difficultyText, section3, 40, 24, BLACK);

    DrawText("Note Mode", section4, 15, 18, DARKGRAY);
    DrawText(game->noteMode ? "ON" : "OFF", section4, 40, 24, BLACK);

}

int  render_drawMenu(Rectangle buttons[3]) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    ClearBackground(RAYWHITE);
    int titleSize = 50;
    int titleWidth = MeasureText("SUDOKU", titleSize);
    DrawText("SUDOKU", (screenWidth - titleWidth) / 2, 50, titleSize, DARKBLUE);
    int buttonWidth = 220;
    int buttonHeight = 55;
    int spacing = 20;
    int startY = screenHeight / 2 - (buttonHeight * 3 + spacing * 2) / 2;
    buttons[0] = (Rectangle){ (screenWidth - buttonWidth) / 2, startY, buttonWidth, buttonHeight };
    buttons[1] = (Rectangle){ (screenWidth - buttonWidth) / 2, startY + buttonHeight + spacing, buttonWidth, buttonHeight };
    buttons[2] = (Rectangle){ (screenWidth - buttonWidth) / 2, startY + (buttonHeight + spacing) * 2, buttonWidth, buttonHeight };
    const char* buttonLabels[3] = { "EASY", "MEDIUM", "HARD" };
    for (int i = 0; i < 3; i++) {
        DrawRectangleRounded(buttons[i], 0.3f, 10, SKYBLUE);
        DrawRectangleLinesEx(buttons[i], 2, DARKBLUE);
        int fontSize = 24;
        int textWidth = MeasureText(buttonLabels[i], fontSize);
        DrawText(buttonLabels[i], buttons[i].x + (buttonWidth - textWidth) / 2, buttons[i].y + (buttonHeight - fontSize) / 2, fontSize, WHITE);


    }
    return 3;
}


void render_drawWinScreen(const Game* game) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
    DrawRectangle(GetScreenWidth() / 2 - 150, GetScreenHeight() / 2 - 100, 300, 200, RAYWHITE);
    DrawRectangleLinesEx((Rectangle) { GetScreenWidth() / 2 - 150, GetScreenHeight() / 2 - 100, 300, 200 }, 2, BLACK);
    int titleSize = 35;
    int titleWidth = MeasureText("YOU WIN!", titleSize);
    DrawText("YOU WIN!", GetScreenWidth() / 2 - titleWidth / 2, GetScreenHeight() / 2 - 70, 35, DARKGREEN);
    int totalSeconds = (int)game->elapsedTime;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    int timeSize = 24;
    int timeWidth = MeasureText(TextFormat("Time: %02d:%02d", minutes, seconds), timeSize);
    DrawText(TextFormat("Time: %02d:%02d", minutes, seconds), GetScreenWidth() / 2 - timeWidth / 2, GetScreenHeight() / 2 - 10, 24, BLACK);
    int subSize = 20;
    int subWidth = MeasureText("Press R to Restart", subSize);
    DrawText("Press R to Restart", GetScreenWidth() / 2 - subWidth / 2, GetScreenHeight() / 2 + 45, 20, DARKGRAY);
}

void render_drawGameOverScreen(const Game* game) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
    DrawRectangle(GetScreenWidth() / 2 - 150, GetScreenHeight() / 2 - 100, 300, 200, RAYWHITE);
    DrawRectangleLinesEx((Rectangle) { GetScreenWidth() / 2 - 150, GetScreenHeight() / 2 - 100, 300, 200 }, 2, BLACK);
    int titleSize = 35;
    int titleWidth = MeasureText("GAME OVER", titleSize);
    DrawText("GAME OVER", GetScreenWidth() / 2 - titleWidth / 2, GetScreenHeight() / 2 - 70, 35, RED);
    int infoSize = 24;
    int infoWidth = MeasureText(TextFormat("Mistakes: %d/%d", game->mistakes, MAX_MISTAKES), infoSize);
    DrawText(TextFormat("Mistakes: %d/%d", game->mistakes, MAX_MISTAKES), GetScreenWidth() / 2 - infoWidth / 2, GetScreenHeight() / 2 - 10, 24, BLACK);
    int subSize = 20;
    int subWidth = MeasureText("Press R to Restart", subSize);
    DrawText("Press R to Restart", GetScreenWidth() / 2 - subWidth / 2, GetScreenHeight() / 2 + 45, 20, DARKGRAY);
}


void render_drawPauseScreen(void) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
    DrawRectangle(GetScreenWidth() / 2 - 150, GetScreenHeight() / 2 - 100, 300, 200, RAYWHITE);
    DrawRectangleLinesEx((Rectangle) { GetScreenWidth() / 2 - 150, GetScreenHeight() / 2 - 100, 300, 200 }, 2, BLACK);
    int fontSize = 35;
    int textWidth = MeasureText("PAUSED", fontSize);
    DrawText("PAUSED", GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 2 - 70, fontSize, DARKBLUE);
    int subFontSize = 20;
    int subTextWidth = MeasureText("Press P to Resume", subFontSize);
    DrawText("Press P to Resume", GetScreenWidth() / 2 - subTextWidth / 2, GetScreenHeight() / 2 + 45, 20, DARKGRAY);

}

