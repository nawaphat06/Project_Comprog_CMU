#ifndef SLOT_UI_H
#define SLOT_UI_H

#include "raylib.h"
#include "Player.h" // ต้องมีไฟล์ Player ของคุณด้วย
#include <vector>
#include <string>

using namespace std;

// --- ดึง Logic ของเพื่อนมาไว้ตรงนี้ ---
struct SlotSymbol {
    string name;
    int value;
    int weight;
};

static vector<SlotSymbol> slotSymbols = {
    {"[GOLD]", 500, 2}, {"[RED]", 100, 6}, {"[BLUE]", 50, 8}, 
    {"[GREEN]", 30, 10}, {"[ K ]", 15, 14}, {"[ Q ]", 10, 15}, 
    {"[ J ]", 8, 20},  {"[ 10 ]", 5 , 25}
};

static SlotSymbol spinSlotSymbol() {
    int totalWeight = 0;
    for (const auto& s : slotSymbols) totalWeight += s.weight;
    int rnd = rand() % totalWeight;
    for (const auto& s : slotSymbols) {
        if (rnd < s.weight) return s;
        rnd -= s.weight;
    }
    return slotSymbols.back();
}

// --- ฟังก์ชันหลักที่จะถูกเรียกจาก Main ---
void playSlotUI(Player &p) {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    
    // ตั้งค่าตัวแปรจากข้อมูล Player จริง
    int bet = 10;
    vector<vector<SlotSymbol>> board(3, vector<SlotSymbol>(3, slotSymbols[7])); 
    
    bool isSpinning = false;
    float spinTimer = 0.0f;
    int multiplier = 1;
    bool showCombo = false;
    string sysMsg = "Press SPIN to start!";
    Color msgColor = RAYWHITE;

    Rectangle btnSpin     = { (screenWidth / 2) - 130.0f, 610.0f, 260.0f, 60.0f };
    Rectangle btnBetMinus = { btnSpin.x - 80.0f, 610.0f, 60.0f, 60.0f };
    Rectangle btnBetPlus  = { btnSpin.x + btnSpin.width + 20.0f, 610.0f, 60.0f, 60.0f };
    Rectangle btnBack     = { 30.0f, 30.0f, 100.0f, 40.0f };

    // ลูปภายในของเกมสล็อต
    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        bool isClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (!isSpinning) {
            if (isClick) {
                if (CheckCollisionPointRec(mousePos, btnBetMinus) && bet > 10) bet -= 10;
                else if (CheckCollisionPointRec(mousePos, btnBetPlus) && bet + 10 <= p.credit) bet += 10;
                else if (CheckCollisionPointRec(mousePos, btnBack)) return; // ออกจากฟังก์ชันกลับไปเมนูหลัก
                
                else if (CheckCollisionPointRec(mousePos, btnSpin)) {
                    if (p.credit >= bet) {
                        p.credit -= bet; 
                        isSpinning = true;
                        spinTimer = 0.0f;
                        multiplier = 1;
                        showCombo = false;
                        sysMsg = "Spinning...";
                    }
                }
            }
        } else {
            spinTimer += GetFrameTime();
            for(int i=0; i<3; i++)
                for(int j=0; j<3; j++) board[i][j] = spinSlotSymbol();

            if (spinTimer >= 1.5f) {
                isSpinning = false;
                int totalWinRound = 0;
                bool hasWinLine = false;
                for (int i = 0; i < 3; i++) {
                    if (board[i][0].name == board[i][1].name && board[i][1].name == board[i][2].name) {
                        totalWinRound += board[i][0].value * multiplier * bet;
                        hasWinLine = true;
                    }
                }
                if (hasWinLine) {
                    p.credit += totalWinRound;
                    p.win_count++;
                    sysMsg = TextFormat("WIN! +$%d!", totalWinRound);
                    msgColor = GREEN;
                    multiplier++;
                    showCombo = true;
                } else {
                    p.loss_count++;
                    sysMsg = "No win this time.";
                    msgColor = LIGHTGRAY;
                    multiplier = 1;
                    showCombo = false;
                }
            }
        }

        BeginDrawing();
        ClearBackground(DARKBLUE);
        DrawText("ASSTECS SLOT GAMES", (screenWidth - MeasureText("ASSTECS SLOT GAMES", 50)) / 2, 30, 50, GOLD);
        DrawText(TextFormat("Balance: $%.2f", p.credit), (screenWidth - MeasureText(TextFormat("Balance: $%.2f", p.credit), 30)) / 2, 90, 30, WHITE);
        
        // วาดกระดานและปุ่ม (ใช้ Logic เดิมที่คุณเทสผ่านแล้ว)
        int boxSize = 120;
        int startX = (screenWidth - (boxSize * 3 + 20)) / 2;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                DrawRectangle(startX + (j * 130), 160 + (i * 130), boxSize, boxSize, RAYWHITE);
                DrawText(board[i][j].name.c_str(), startX + (j * 130) + 20, 160 + (i * 130) + 50, 24, BLACK);
            }
        }

        DrawRectangleRec(btnBetMinus, CheckCollisionPointRec(mousePos, btnBetMinus) ? LIGHTGRAY : GRAY);
        DrawRectangleRec(btnBetPlus, CheckCollisionPointRec(mousePos, btnBetPlus) ? LIGHTGRAY : GRAY);
        DrawRectangleRec(btnSpin, isSpinning ? DARKGRAY : (CheckCollisionPointRec(mousePos, btnSpin) ? LIGHTGRAY : GRAY));
        DrawText(TextFormat("Bet: %d", bet), (screenWidth - MeasureText(TextFormat("Bet: %d", bet), 30)) / 2, 560, 30, WHITE);
        DrawText("SPIN", btnSpin.x + 85, btnSpin.y + 15, 35, BLACK);
        DrawText(sysMsg.c_str(), (screenWidth - MeasureText(sysMsg.c_str(), 25)) / 2, 680, 25, msgColor);
        DrawRectangleRec(btnBack, RED); DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);
        
        EndDrawing();
    }
}

#endif