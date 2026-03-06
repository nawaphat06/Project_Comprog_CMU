#ifndef HILO_UI_H
#define HILO_UI_H

#include "raylib.h"
#include "Player.h"
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

// --- ลอจิกการทอยเต๋า ---
inline int rollDiceLogic() { return rand() % 6 + 1; }

// --- ฟังก์ชันวาดลูกเต๋าสมจริง ---
static void DrawDiceUI(float x, float y, float size, int value) {
    DrawRectangle(x, y, size, size, RAYWHITE);
    DrawRectangleLinesEx({x, y, size, size}, 4, BLACK);
    
    float d = size / 4.0f; 
    float c = size / 2.0f; 
    float r = size / 10.0f; 
    
    Color dotC = BLACK;
    if (value == 1) { 
        DrawCircle(x+c, y+c, r*1.5f, RED); 
    } else if (value == 2) { 
        DrawCircle(x+d, y+d, r, dotC); DrawCircle(x+size-d, y+size-d, r, dotC); 
    } else if (value == 3) { 
        DrawCircle(x+d, y+d, r, dotC); DrawCircle(x+c, y+c, r, RED); DrawCircle(x+size-d, y+size-d, r, dotC); 
    } else if (value == 4) { 
        DrawCircle(x+d, y+d, r, dotC); DrawCircle(x+size-d, y+d, r, dotC); 
        DrawCircle(x+d, y+size-d, r, dotC); DrawCircle(x+size-d, y+size-d, r, dotC); 
    } else if (value == 5) { 
        DrawCircle(x+d, y+d, r, dotC); DrawCircle(x+size-d, y+d, r, dotC); DrawCircle(x+c, y+c, r, RED); 
        DrawCircle(x+d, y+size-d, r, dotC); DrawCircle(x+size-d, y+size-d, r, dotC); 
    } else if (value == 6) { 
        DrawCircle(x+d, y+d, r, dotC); DrawCircle(x+size-d, y+d, r, dotC); 
        DrawCircle(x+d, y+c, r, dotC); DrawCircle(x+size-d, y+c, r, dotC); 
        DrawCircle(x+d, y+size-d, r, dotC); DrawCircle(x+size-d, y+size-d, r, dotC); 
    }
}

void playHiloUI(Player &p) {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;

    // --- โหลดรูปภาพ How to play สำหรับ Hi-Lo ---
    Texture2D htpHL = LoadTexture("howtoplay/hl.png");

    int choice = 1;      
    int hlChoice = 1;    
    int guessTriple = 1; 
    int guessSum = 10;   
    int luckyNumber = 1; 
    int winStreak = 0;
    
    int bet = 0;
    string betInput = "50"; 
    bool betBoxActive = false;

    int dice[3] = {1, 1, 1};
    bool isRolling = false;
    float rollTimer = 0.0f;
    string sysMsg = "Adjust Bet, select Mode/Lucky No, then ROLL!";
    Color msgColor = RAYWHITE;

    // gameState: 0 = normal, 3 = How To Play
    int hiloState = 0; 

    Rectangle btnRoll   = { centerX - 120, 620, 240, 60 };
    Rectangle boxBet    = { 380, 515, 200, 45 }; 
    Rectangle btnBack   = { 30, 30, 100, 40 };
    Rectangle btnHTP    = { 30, 80, 150, 40 }; // ปุ่มวิธีเล่น

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        bool isClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        Rectangle rMode1 = { 380, 280, 180, 45 }; 
        Rectangle rMode2 = { 580, 280, 180, 45 }; 
        Rectangle rMode3 = { 780, 280, 180, 45 }; 

        if (hiloState == 0) { // หน้าเล่นปกติ
            if (!isRolling) {
                if (isClick) {
                    if (CheckCollisionPointRec(mousePos, btnBack)) {
                        UnloadTexture(htpHL);
                        return;
                    }
                    if (CheckCollisionPointRec(mousePos, btnHTP)) hiloState = 3;

                    if (CheckCollisionPointRec(mousePos, boxBet)) betBoxActive = true;
                    else betBoxActive = false;

                    if (CheckCollisionPointRec(mousePos, rMode1)) choice = 1;
                    if (CheckCollisionPointRec(mousePos, rMode2)) choice = 2;
                    if (CheckCollisionPointRec(mousePos, rMode3)) choice = 3;

                    if (choice == 1) {
                        if (CheckCollisionPointRec(mousePos, { 380, 345, 150, 40 })) hlChoice = 1;
                        if (CheckCollisionPointRec(mousePos, { 550, 345, 150, 40 })) hlChoice = 2;
                    } else if (choice == 2) {
                        for(int i=1; i<=6; i++) 
                            if (CheckCollisionPointRec(mousePos, { 380.0f + ((i-1)*70), 345, 60, 40 })) guessTriple = i;
                    } else if (choice == 3) {
                        if (CheckCollisionPointRec(mousePos, { 380, 345, 60, 40 }) && guessSum > 3) guessSum--;
                        if (CheckCollisionPointRec(mousePos, { 600, 345, 60, 40 }) && guessSum < 18) guessSum++;
                    }

                    for(int i=1; i<=6; i++)
                        if (CheckCollisionPointRec(mousePos, { 380.0f + ((i-1)*70), 430, 60, 40 })) luckyNumber = i;

                    if (CheckCollisionPointRec(mousePos, btnRoll)) {
                        bet = atoi(betInput.c_str()); 
                        if (bet <= 0) { sysMsg = "Bet must be > 0!"; msgColor = RED; }
                        else if (p.credit >= bet) {
                            p.credit -= bet; isRolling = true; betBoxActive = false; rollTimer = 0.0f;
                            sysMsg = "Shaking..."; msgColor = RAYWHITE;
                        } else { sysMsg = "Not enough credits!"; msgColor = RED; }
                    }
                }

                if (betBoxActive) {
                    int key = GetCharPressed();
                    while (key > 0) {
                        if ((key >= '0') && (key <= '9') && betInput.length() < 7) betInput += (char)key;
                        key = GetCharPressed(); 
                    }
                    if (IsKeyPressed(KEY_BACKSPACE) && betInput.length() > 0) betInput.pop_back();
                }
            } else {
                rollTimer += GetFrameTime();
                dice[0] = rollDiceLogic(); dice[1] = rollDiceLogic(); dice[2] = rollDiceLogic();
                
                if (rollTimer >= 1.5f) {
                    isRolling = false;
                    int sum = dice[0] + dice[1] + dice[2];
                    bool win = false; int reward = 0;

                    if (choice == 1) {
                        if (!(dice[0] == dice[1] && dice[1] == dice[2])) { 
                            if (hlChoice == 1 && sum >= 4 && sum <= 10) { win = true; reward = bet * 2; }
                            else if (hlChoice == 2 && sum >= 11 && sum <= 17) { win = true; reward = bet * 2; }
                        }
                    } else if (choice == 2) {
                        if (dice[0] == dice[1] && dice[1] == dice[2] && dice[0] == guessTriple) { win = true; reward = bet * 5; }
                    } else if (choice == 3) {
                        if (sum == guessSum) { win = true; reward = bet * 3; }
                    }

                    int luckyCount = 0;
                    for(int i=0; i<3; i++) if (dice[i] == luckyNumber) luckyCount++;

                    if (win) {
                        p.credit += reward; winStreak++; p.win_count++;
                        sysMsg = TextFormat("WIN! +$%d (Total)", reward + (luckyCount * bet)); msgColor = GREEN;
                    } else {
                        winStreak = 0; p.loss_count++;
                        if (luckyCount > 0) { sysMsg = TextFormat("LOSE.. But Lucky No gave +$%d", luckyCount * bet); msgColor = YELLOW; }
                        else { sysMsg = "LOSE!"; msgColor = RED; }
                    }
                    p.credit += (luckyCount * bet);
                }
            }
        } else if (hiloState == 3) { // หน้า How to play
            if (isClick && CheckCollisionPointRec(mousePos, btnBack)) hiloState = 0;
        }

        BeginDrawing();
        ClearBackground(Color{ 10, 60, 20, 255 }); 

        if (hiloState == 3) {
            if (htpHL.id != 0) DrawTexture(htpHL, 0, 0, WHITE);
            else DrawText("IMAGE NOT FOUND: howtoplay/hl.png", centerX - 250, centerY, 20, RED);

            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON); 
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);
        } else {
            DrawText("HI-LO CASINO", (screenWidth - MeasureText("HI-LO CASINO", 40)) / 2, 20, 40, GOLD);
            DrawText(TextFormat("Credits: $%.2f | Streak: %d", p.credit, winStreak), (screenWidth - MeasureText(TextFormat("Credits: $%.2f | Streak: %d", p.credit, winStreak), 25)) / 2, 70, 25, WHITE);

            int diceBoxSize = 100;
            int startDiceX = centerX - (diceBoxSize * 1.5f) - 20; 
            for (int i = 0; i < 3; i++) DrawDiceUI(startDiceX + (i * 140), 120, diceBoxSize, dice[i]);
            
            DrawText(sysMsg.c_str(), (screenWidth - MeasureText(sysMsg.c_str(), 25)) / 2, 235, 25, msgColor);
            DrawRectangle(140, 270, 1000, 315, Color{0, 0, 0, 100});

            // SELECT MODE
            DrawText("1. SELECT MODE:", 180, 290, 20, WHITE);
            DrawRectangleRec(rMode1, (choice == 1) ? GOLD : GRAY); DrawText("High/Low", rMode1.x+45, rMode1.y+12, 20, BLACK);
            DrawRectangleRec(rMode2, (choice == 2) ? GOLD : GRAY); DrawText("Triple", rMode2.x+60, rMode2.y+12, 20, BLACK);
            DrawRectangleRec(rMode3, (choice == 3) ? GOLD : GRAY); DrawText("Sum", rMode3.x+70, rMode3.y+12, 20, BLACK);

            if (choice == 1) {
                DrawRectangleRec({ 380, 345, 150, 40 }, (hlChoice == 1) ? ORANGE : GRAY); DrawText("LOW", 430, 355, 20, BLACK);
                DrawRectangleRec({ 550, 345, 150, 40 }, (hlChoice == 2) ? ORANGE : GRAY); DrawText("HIGH", 595, 355, 20, BLACK);
            } else if (choice == 2) {
                for(int i=1; i<=6; i++) {
                    Rectangle r = { 380.0f + ((i-1)*70), 345, 60, 40 };
                    DrawRectangleRec(r, (guessTriple == i) ? ORANGE : GRAY); DrawText(TextFormat("%d", i), r.x+25, r.y+10, 20, BLACK);
                }
            } else if (choice == 3) {
                DrawRectangleRec({ 380, 345, 60, 40 }, GRAY); DrawText("-", 403, 350, 30, BLACK);
                DrawText(TextFormat("SUM: %d", guessSum), 465, 350, 25, YELLOW);
                DrawRectangleRec({ 600, 345, 60, 40 }, GRAY); DrawText("+", 620, 350, 30, BLACK);
            }

            // LUCKY NO
            DrawText("2. LUCKY NO:", 180, 440, 20, SKYBLUE);
            for(int i=1; i<=6; i++) {
                Rectangle r = { 380.0f + ((i-1)*70), 430, 60, 40 };
                DrawRectangleRec(r, (luckyNumber == i) ? SKYBLUE : GRAY); DrawText(TextFormat("%d", i), r.x+25, r.y+10, 20, BLACK);
            }

            // BET AMOUNT
            DrawText("3. BET AMOUNT:", 180, 525, 20, PINK);
            DrawRectangleRec(boxBet, betBoxActive ? RAYWHITE : LIGHTGRAY);
            DrawText(betInput.c_str(), boxBet.x+15, boxBet.y+8, 30, BLACK);

            // ROLL BUTTON
            DrawRectangleRec(btnRoll, isRolling ? DARKGRAY : RED);
            DrawRectangleLinesEx(btnRoll, 4, isRolling ? LIGHTGRAY : GOLD);
            DrawText(isRolling ? "SHAKING..." : "ROLL DICE", btnRoll.x+60, btnRoll.y+18, 25, GOLD);

            // BACK & HTP
            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON); 
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);
            DrawRectangleRec(btnHTP, CheckCollisionPointRec(mousePos, btnHTP) ? GRAY : DARKGRAY);
            DrawText("HOW TO PLAY", btnHTP.x + 15, btnHTP.y + 10, 18, RAYWHITE);
        }
        EndDrawing();
    }
    UnloadTexture(htpHL);
}
#endif