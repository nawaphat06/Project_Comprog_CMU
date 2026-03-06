#ifndef SLOT_UI_H
#define SLOT_UI_H

#include "raylib.h"
#include "Player.h" 
#include <vector>
#include <string>
#include <cstdlib> 
#include <algorithm>

using namespace std;

struct SlotSymbol {
    string name;
    int value;
    int weight;
    Color color; 
};

static vector<SlotSymbol> slotSymbols = {
    {"777", 500, 2, GOLD},         
    {"BAR", 100, 6, RED},          
    {"DIA", 50, 8, SKYBLUE},       
    {"CHRY", 30, 10, MAGENTA},     
    {" K ", 15, 14, DARKGRAY}, 
    {" Q ", 10, 15, DARKGRAY}, 
    {" J ", 8, 20, DARKGRAY},  
    {" 10 ", 5 , 25, DARKGRAY}
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

void playSlotUI(Player &p) {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f; // แก้ Error centerY

    // --- โหลดรูปภาพ How to play สำหรับ Slot ---
    Texture2D htpSlot = LoadTexture("howtoplay/sl.png");

    int bet = 0;
    string betInput = "10"; 
    bool betBoxActive = false;

    vector<vector<SlotSymbol>> board(3, vector<SlotSymbol>(3, slotSymbols[7])); 
    
    bool winCells[3][3] = { {false, false, false}, 
                            {false, false, false}, 
                            {false, false, false} };

    // gameState: 0 = normal, 3 = How To Play
    int slotState = 0; 
    bool isSpinning = false;
    float spinTimer = 0.0f;
    int multiplier = 1;
    string sysMsg = "Enter Bet and Press SPIN!";
    Color msgColor = RAYWHITE;

    Rectangle boxBet   = { centerX - 100, 585, 200, 45 }; 
    Rectangle btnSpin  = { centerX - 130, 645, 260, 55 };
    Rectangle btnBack  = { 30.0f, 30.0f, 100.0f, 40.0f };
    Rectangle btnHTP   = { 30.0f, 80.0f, 150.0f, 40.0f }; // ปุ่มวิธีเล่น

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        bool isClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (slotState == 0) { // หน้าเล่นปกติ
            if (!isSpinning) {
                if (isClick) {
                    if (CheckCollisionPointRec(mousePos, btnBack)) {
                        UnloadTexture(htpSlot);
                        return; 
                    }
                    if (CheckCollisionPointRec(mousePos, btnHTP)) slotState = 3;
                    
                    if (CheckCollisionPointRec(mousePos, boxBet)) betBoxActive = true;
                    else betBoxActive = false;
                    
                    if (CheckCollisionPointRec(mousePos, btnSpin)) {
                        bet = atoi(betInput.c_str()); 
                        if (bet <= 0) {
                            sysMsg = "Bet must be > 0!";
                            msgColor = RED;
                        } else if (p.credit >= bet) {
                            p.credit -= bet; 
                            isSpinning = true;
                            betBoxActive = false; 
                            spinTimer = 0.0f;
                            multiplier = 1;
                            sysMsg = "Spinning...";
                            msgColor = RAYWHITE;
                            for(int i=0; i<3; i++) for(int j=0; j<3; j++) winCells[i][j] = false;
                        } else {
                            sysMsg = "Not enough credits!";
                            msgColor = RED;
                        }
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
                spinTimer += GetFrameTime();
                for(int i=0; i<3; i++) for(int j=0; j<3; j++) board[i][j] = spinSlotSymbol();

                if (spinTimer >= 1.5f) {
                    isSpinning = false;
                    int totalWinRound = 0;
                    bool hasWinLine = false;

                    for (int i = 0; i < 3; i++) {
                        if (board[i][0].name == board[i][1].name && board[i][1].name == board[i][2].name) {
                            totalWinRound += board[i][0].value * bet;
                            hasWinLine = true;
                            winCells[i][0] = true; winCells[i][1] = true; winCells[i][2] = true;
                        }
                    }
                    if (board[0][0].name == board[1][1].name && board[1][1].name == board[2][2].name) {
                        totalWinRound += board[0][0].value * bet;
                        hasWinLine = true;
                        winCells[0][0] = true; winCells[1][1] = true; winCells[2][2] = true;
                    }
                    if (board[2][0].name == board[1][1].name && board[1][1].name == board[0][2].name) {
                        totalWinRound += board[2][0].value * bet;
                        hasWinLine = true;
                        winCells[2][0] = true; winCells[1][1] = true; winCells[0][2] = true;
                    }

                    if (hasWinLine) {
                        p.credit += totalWinRound; p.win_count++;
                        sysMsg = TextFormat("WIN! +$%d!", totalWinRound);
                        msgColor = GREEN;
                    } else {
                        p.loss_count++;
                        sysMsg = "No win this time.";
                        msgColor = LIGHTGRAY;
                    }
                }
            }
        } else if (slotState == 3) { // หน้า How to play
            if (isClick && CheckCollisionPointRec(mousePos, btnBack)) slotState = 0;
        }

        BeginDrawing();
        ClearBackground(Color{ 10, 30, 70, 255 }); 
        
        if (slotState == 3) {
            if (htpSlot.id != 0) DrawTexture(htpSlot, 0, 0, WHITE);
            else DrawText("IMAGE NOT FOUND: howtoplay/sl.png", centerX - 250, centerY, 20, RED);

            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON); 
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);
        } else {
            DrawText("SCAMMER SLOT MACHINE", (screenWidth - MeasureText("SCAMMER SLOT MACHINE", 50)) / 2, 30, 50, GOLD);
            DrawText(TextFormat("Balance: $%.2f", p.credit), (screenWidth - MeasureText(TextFormat("Balance: $%.2f", p.credit), 30)) / 2, 90, 30, RAYWHITE);
            
            int bSize = 120;
            int spc = 10;
            int gW = (bSize * 3) + (spc * 2);
            int stX = (screenWidth - gW) / 2;
            int stY = 140;

            DrawRectangle(stX - 20, stY - 20, gW + 40, (bSize * 3) + (spc * 2) + 40, Color{ 5, 15, 40, 255 }); 
            DrawRectangleLinesEx({(float)stX - 20, (float)stY - 20, (float)gW + 40, (float)(bSize * 3) + (spc * 2) + 40}, 6, GOLD); 

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int cellX = stX + (j * (bSize + spc));
                    int cellY = stY + (i * (bSize + spc));
                    DrawRectangle(cellX, cellY, bSize, bSize, winCells[i][j] ? LIME : RAYWHITE);
                    DrawText(board[i][j].name.c_str(), cellX + (bSize - MeasureText(board[i][j].name.c_str(), 35))/2, cellY + 40, 35, winCells[i][j] ? BLACK : board[i][j].color);
                }
            }

            DrawText(sysMsg.c_str(), (screenWidth - MeasureText(sysMsg.c_str(), 30)) / 2, 550, 30, msgColor);

            if (!isSpinning) {
                DrawText("BET AMOUNT:", boxBet.x - 140, boxBet.y + 12, 20, RAYWHITE);
                DrawRectangleRec(boxBet, betBoxActive ? RAYWHITE : LIGHTGRAY);
                DrawText(betInput.c_str(), boxBet.x + 15, boxBet.y + 8, 30, BLACK);
                
                DrawRectangleRec(btnHTP, CheckCollisionPointRec(mousePos, btnHTP) ? GRAY : DARKGRAY);
                DrawText("HOW TO PLAY", btnHTP.x + 15, btnHTP.y + 10, 18, RAYWHITE);
            }

            DrawRectangleRec(btnSpin, isSpinning ? DARKGRAY : RED);
            DrawRectangleLinesEx(btnSpin, 4, GOLD);
            DrawText(isSpinning ? "SPINNING..." : "SPIN", btnSpin.x + (btnSpin.width - MeasureText(isSpinning ? "SPINNING..." : "SPIN", 35))/2, btnSpin.y + 10, 35, GOLD);
            
            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON); 
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);
        }
        EndDrawing();
    }
    UnloadTexture(htpSlot);
}

#endif