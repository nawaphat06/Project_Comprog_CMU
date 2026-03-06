#ifndef HILO_UI_H
#define HILO_UI_H

#include "raylib.h"
#include "Player.h"
#include <string>
#include <vector>
#include <ctime>

using namespace std;

// --- ลอจิกการทอยเต๋า ---
inline int rollDiceLogic() { return rand() % 6 + 1; }

void playHiloUI(Player &p) {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    // --- ตัวแปรสถานะเกม ---
    int bet = 50;
    int choice = 1;      // 1: High/Low, 2: Triple, 3: Sum
    int hlChoice = 1;    // 1: Low, 2: High
    int guessTriple = 1; // 1-6
    int guessSum = 10;   // 3-18
    int luckyNumber = 1; // 1-6
    int winStreak = 0;
    
    int dice[3] = {1, 1, 1};
    bool isRolling = false;
    float rollTimer = 0.0f;
    string sysMsg = "Adjust Bet, select Mode/Lucky No, then ROLL!";
    Color msgColor = RAYWHITE;

    // --- พิกัดปุ่มหลัก ---
    Rectangle btnRoll     = { 540, 620, 200, 60 };
    Rectangle btnBetMinus = { 530, 520, 50, 50 }; 
    Rectangle btnBetPlus  = { 700, 520, 50, 50 }; 
    Rectangle btnBack     = { 30, 30, 100, 40 };

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        bool isClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (!isRolling) {
            if (isClick) {
                // 1. ระบบปรับ Bet
                if (CheckCollisionPointRec(mousePos, btnBetMinus) && bet > 10) bet -= 10;
                if (CheckCollisionPointRec(mousePos, btnBetPlus) && bet + 10 <= p.credit) bet += 10;

                // 2. เลือกโหมด (พิกัด X ใหม่: 400, 610, 820)
                if (CheckCollisionPointRec(mousePos, { 400, 280, 180, 50 })) choice = 1;
                if (CheckCollisionPointRec(mousePos, { 610, 280, 180, 50 })) choice = 2;
                if (CheckCollisionPointRec(mousePos, { 820, 280, 180, 50 })) choice = 3;

                // 3. ปรับค่าโหมดรายย่อย
                if (choice == 1) {
                    if (CheckCollisionPointRec(mousePos, { 440, 345, 150, 40 })) hlChoice = 1;
                    if (CheckCollisionPointRec(mousePos, { 690, 345, 150, 40 })) hlChoice = 2;
                } else if (choice == 2) {
                    for(int i=1; i<=6; i++) 
                        if (CheckCollisionPointRec(mousePos, { 350.0f+(i*80), 345, 60, 40 })) guessTriple = i;
                } else if (choice == 3) {
                    if (CheckCollisionPointRec(mousePos, { 540, 345, 60, 40 }) && guessSum > 3) guessSum--;
                    if (CheckCollisionPointRec(mousePos, { 700, 345, 60, 40 }) && guessSum < 18) guessSum++;
                }

                // 4. เลือก Lucky Number
                for(int i=1; i<=6; i++)
                    if (CheckCollisionPointRec(mousePos, { 350.0f+(i*80), 430, 60, 40 })) luckyNumber = i;

                // 5. ปุ่มเริ่มทอย
                if (CheckCollisionPointRec(mousePos, btnRoll)) {
                    if (p.credit >= bet && bet > 0) {
                        isRolling = true;
                        rollTimer = 0.0f;
                        sysMsg = "Shaking...";
                    }
                }

                // 6. ปุ่มกลับเมนูหลัก
                if (CheckCollisionPointRec(mousePos, btnBack)) return;
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
                    p.credit += (reward - bet); winStreak++; p.win_count++;
                    sysMsg = TextFormat("WIN! +$%d", reward); msgColor = GREEN;
                } else {
                    p.credit -= bet; winStreak = 0; p.loss_count++;
                    sysMsg = "LOSE!"; msgColor = RED;
                }
                p.credit += (luckyCount * bet);
            }
        }

        BeginDrawing();
        ClearBackground(Color{ 0, 80, 0, 255 });

        // --- หัวข้อ ---
        DrawText("HI-LO CASINO", (screenWidth - MeasureText("HI-LO CASINO", 40)) / 2, 20, 40, GOLD);
        DrawText(TextFormat("Credits: $%.2f | Streak: %d", p.credit, winStreak), (screenWidth - MeasureText(TextFormat("Credits: $%.2f | Streak: %d", p.credit, winStreak), 25)) / 2, 70, 25, WHITE);

        // --- ลูกเต๋า ---
        for (int i = 0; i < 3; i++) {
            int posX = 440 + (i * 150);
            DrawRectangle(posX, 120, 100, 100, RAYWHITE);
            DrawText(TextFormat("%d", dice[i]), posX + 35, 140, 60, BLACK);
        }
        DrawText(sysMsg.c_str(), (screenWidth - MeasureText(sysMsg.c_str(), 25)) / 2, 240, 25, msgColor);

        // --- 1. SELECT MODE ---
        DrawText("1. SELECT MODE:", 180, 290, 20, WHITE);
        
        Rectangle rMode1 = { 400, 280, 180, 50 }; 
        DrawRectangleRec(rMode1, (choice == 1) ? GOLD : (CheckCollisionPointRec(mousePos, rMode1) ? LIGHTGRAY : GRAY));
        DrawText("High/Low", 440, 295, 20, BLACK);

        Rectangle rMode2 = { 610, 280, 180, 50 }; 
        DrawRectangleRec(rMode2, (choice == 2) ? GOLD : (CheckCollisionPointRec(mousePos, rMode2) ? LIGHTGRAY : GRAY));
        DrawText("Triple", 665, 295, 20, BLACK);

        Rectangle rMode3 = { 820, 280, 180, 50 }; 
        DrawRectangleRec(rMode3, (choice == 3) ? GOLD : (CheckCollisionPointRec(mousePos, rMode3) ? LIGHTGRAY : GRAY));
        DrawText("Sum", 885, 295, 20, BLACK);

        // --- รายละเอียด Mode ---
        if (choice == 1) {
            Rectangle rHL1 = { 440, 345, 150, 40 };
            DrawRectangleRec(rHL1, (hlChoice == 1) ? ORANGE : (CheckCollisionPointRec(mousePos, rHL1) ? LIGHTGRAY : GRAY));
            DrawText("LOW", 495, 355, 20, BLACK);
            Rectangle rHL2 = { 690, 345, 150, 40 };
            DrawRectangleRec(rHL2, (hlChoice == 2) ? ORANGE : (CheckCollisionPointRec(mousePos, rHL2) ? LIGHTGRAY : GRAY));
            DrawText("HIGH", 740, 355, 20, BLACK);
        } else if (choice == 2) {
            for(int i=1; i<=6; i++) {
                Rectangle rTrip = { 350.0f+(i*80), 345, 60, 40 };
                DrawRectangleRec(rTrip, (guessTriple == i) ? ORANGE : (CheckCollisionPointRec(mousePos, rTrip) ? LIGHTGRAY : GRAY));
                DrawText(TextFormat("%d", i), 375.0f+(i*80), 355, 20, BLACK);
            }
        } else if (choice == 3) {
            Rectangle rMinus = { 540, 345, 60, 40 };
            DrawRectangleRec(rMinus, CheckCollisionPointRec(mousePos, rMinus) ? LIGHTGRAY : GRAY);
            DrawText("-", 565, 350, 30, BLACK);
            DrawText(TextFormat("SUM: %d", guessSum), 610, 355, 25, YELLOW);
            Rectangle rPlus = { 700, 345, 60, 40 };
            DrawRectangleRec(rPlus, CheckCollisionPointRec(mousePos, rPlus) ? LIGHTGRAY : GRAY);
            DrawText("+", 720, 350, 30, BLACK);
        }

        // --- 2. LUCKY NO ---
        DrawText("2. LUCKY NO:", 180, 440, 20, SKYBLUE);
        for(int i=1; i<=6; i++) {
            Rectangle rLucky = { 350.0f+(i*80), 430, 60, 40 };
            DrawRectangleRec(rLucky, (luckyNumber == i) ? SKYBLUE : (CheckCollisionPointRec(mousePos, rLucky) ? LIGHTGRAY : GRAY));
            DrawText(TextFormat("%d", i), 375.0f+(i*80), 440, 20, BLACK);
        }

        // --- 3. BET AMOUNT ---
        DrawText("3. BET AMOUNT:", 180, 535, 20, PINK);
        DrawRectangleRec(btnBetMinus, CheckCollisionPointRec(mousePos, btnBetMinus) ? LIGHTGRAY : GRAY); 
        DrawText("-", btnBetMinus.x + 18, btnBetMinus.y + 10, 30, BLACK);
        DrawText(TextFormat("$ %d", bet), 605, 530, 30, WHITE);
        DrawRectangleRec(btnBetPlus, CheckCollisionPointRec(mousePos, btnBetPlus) ? LIGHTGRAY : GRAY); 
        DrawText("+", btnBetPlus.x + 15, btnBetPlus.y + 10, 30, BLACK);

        // --- ROLL & BACK ---
        DrawRectangleRec(btnRoll, isRolling ? DARKGRAY : (CheckCollisionPointRec(mousePos, btnRoll) ? LIGHTGRAY : GRAY));
        DrawText("ROLL DICE", btnRoll.x + 40, btnRoll.y + 15, 25, BLACK);
        DrawRectangleRec(btnBack, MAROON); 
        DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);

        EndDrawing();
    }
}

#endif