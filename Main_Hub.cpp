#include <iostream>
#include <string>

// นำเข้าข้อมูลผู้เล่น
#include "Player.h"

#include "raylib.h"

// นำเข้าไฟล์ UI ของแต่ละเกม
#include "slot.h" 
#include "hilo.h" 
#include "blackjack.h" 
#include "bingo.h" 


using namespace std;

int main() {
    // 1. ตั้งค่าหน้าต่างโปรแกรม
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Asstecs Casino - Final Edition");
    SetTargetFPS(60);

    // 2. สร้างตัวละครผู้เล่น
    Player p1("Guest Player", 1000.0, 0, 0);

    // 3. กำหนดพิกัดปุ่มเมนูหลัก
    Rectangle btnSlot      = { 490, 240, 300, 55 };
    Rectangle btnHilo      = { 490, 310, 300, 55 };
    Rectangle btnBlackjack = { 490, 380, 300, 55 };
    Rectangle btnBingo     = { 490, 450, 300, 55 };
    Rectangle btnExit      = { 490, 540, 300, 55 };

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (CheckCollisionPointRec(mousePos, btnSlot))      playSlotUI(p1); 
            if (CheckCollisionPointRec(mousePos, btnHilo))      playHiloUI(p1); 
            if (CheckCollisionPointRec(mousePos, btnBlackjack)) playBlackjackUI(p1); 
            if (CheckCollisionPointRec(mousePos, btnBingo))     playBingoUI(p1); 
            if (CheckCollisionPointRec(mousePos, btnExit))      break;
        }

        BeginDrawing();
        ClearBackground(DARKGREEN); 
        
        const char* title = "ASSTECS CASINO HUB";
        DrawText(title, (screenWidth - MeasureText(title, 50)) / 2, 60, 50, GOLD);
        
        DrawText(TextFormat("Welcome: %s", p1.name.c_str()), 490, 140, 25, RAYWHITE);
        DrawText(TextFormat("Total Credits: $%.2f", p1.credit), 490, 180, 30, YELLOW);

        // วาดปุ่มต่างๆ
        DrawRectangleRec(btnSlot, CheckCollisionPointRec(mousePos, btnSlot) ? LIGHTGRAY : GRAY);
        DrawText("PLAY SLOT MACHINE", btnSlot.x + 45, btnSlot.y + 18, 20, BLACK);

        DrawRectangleRec(btnHilo, CheckCollisionPointRec(mousePos, btnHilo) ? LIGHTGRAY : GRAY);
        DrawText("PLAY HI-LO GAME", btnHilo.x + 60, btnHilo.y + 18, 20, BLACK);

        DrawRectangleRec(btnBlackjack, CheckCollisionPointRec(mousePos, btnBlackjack) ? LIGHTGRAY : GRAY);
        DrawText("PLAY BLACKJACK 21", btnBlackjack.x + 50, btnBlackjack.y + 18, 20, BLACK);

        DrawRectangleRec(btnBingo, CheckCollisionPointRec(mousePos, btnBingo) ? LIGHTGRAY : GRAY);
        DrawText("PLAY BINGO BONANZA", btnBingo.x + 40, btnBingo.y + 18, 20, BLACK);

        DrawRectangleRec(btnExit, CheckCollisionPointRec(mousePos, btnExit) ? MAROON : RED);
        DrawText("EXIT PROGRAM", btnExit.x + 75, btnExit.y + 18, 20, WHITE);

        DrawText(TextFormat("Lifetime Wins: %d | Lifetime Losses: %d", p1.win_count, p1.loss_count), (screenWidth - MeasureText(TextFormat("Lifetime Wins: %d | Lifetime Losses: %d", p1.win_count, p1.loss_count), 20)) / 2, 660, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}