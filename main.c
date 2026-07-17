#include "raylib.h"
#include "sudoku.h"
#include <time.h>
#include <stdlib.h>

int main(void) {
    const int screenWidth = 600;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Sudoku Game By -H.H.M");


    Game game = {0};
    

    srand((unsigned int)time(NULL)); 
    

    game_reset(&game);

    SetTargetFPS(60);


    while (!WindowShouldClose()) {

        input_handle(&game);
        
        game_update(&game);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (game.state) {
            case GAME_MENU: {
                Rectangle buttons[3];
                render_drawMenu(buttons);
                break;
            }
            case GAME_PLAYING:
                render_drawBoard(&game);
                render_drawHUD(&game);
                break;
            case GAME_PAUSED:
                render_drawBoard(&game);
                render_drawHUD(&game);
                render_drawPauseScreen();
                break;
            case GAME_WIN:
                render_drawBoard(&game);
                render_drawWinScreen(&game);
                break;
            case GAME_OVER:
                render_drawBoard(&game);
                render_drawGameOverScreen(&game);
                break;
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}