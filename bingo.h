#ifndef BINGO_H 
#define BINGO_H 

#include "raylib.h"
#include "Player.h"
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>

using namespace std;

// --- โครงสร้าง Logic เกม Bingo ---
class BingoGame {
public:
    int player_paper[5][5];
    int enemy_paper[5][5];
    bool player_score[5][5];
    bool enemy_score[5][5];
    int win_type; // 0: row, 1: cross, 2: column

    void randbingo(int table[5][5]) {
        vector<int> nums;
        for(int i=1; i<=99; i++) nums.push_back(i);
        
        int n = nums.size();
        for (int i = n - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            swap(nums[i], nums[j]);
        }

        for(int i=0; i<5; i++) {
            for(int j=0; j<5; j++) table[i][j] = nums[i * 5 + j];
        }
        table[2][2] = 0; 
    }

    void checkMatch(int table[5][5], bool score[5][5], int drawnNum) {
        for(int i=0; i<5; i++) {
            for(int j=0; j<5; j++) {
                if(table[i][j] == drawnNum) score[i][j] = true;
            }
        }
        score[2][2] = true; 
    }

    bool checkWin(bool s[5][5], int type) {
        if(type == 0) { 
            for(int i=0; i<5; i++) if(s[i][0] && s[i][1] && s[i][2] && s[i][3] && s[i][4]) return true;
        } else if(type == 1) { 
            if(s[0][0] && s[1][1] && s[2][2] && s[3][3] && s[4][4]) return true;
            if(s[0][4] && s[1][3] && s[2][2] && s[3][1] && s[4][0]) return true;
        } else if(type == 2) { 
            for(int i=0; i<5; i++) if(s[0][i] && s[1][i] && s[2][i] && s[3][i] && s[4][i]) return true;
        }
        return false;
    }
};

void playBingoUI(Player &p) {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    float centerX = screenWidth / 2.0f; // จุดกึ่งกลางหน้าจอ

    BingoGame game;
    int bet = 50;
    bool isPlaying = false;
    bool gameOver = false;
    int lastBall = 0;
    vector<int> ballPool;
    string sysMsg = "Set your Bet and DEAL to get a card!";
    Color msgColor = RAYWHITE;

    // --- ปรับพิกัดปุ่มสำหรับการคลิก (Collision) ---
    Rectangle btnDeal  = { centerX - 100, 600, 200, 60 }; 
    Rectangle btnDraw  = { 950, 450, 200, 60 }; 
    Rectangle btnMinus = { centerX - 125, 520, 50, 50 }; // ขยับปุ่มลบไปทางซ้าย
    Rectangle btnPlus  = { centerX + 75, 520, 50, 50 };  // ขยับปุ่มบวกไปทางขวา
    Rectangle btnBack  = { 30, 30, 100, 40 };

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        bool isClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (!isPlaying && !gameOver) {
            if (isClick) {
                if (CheckCollisionPointRec(mousePos, btnMinus) && bet > 10) bet -= 10;
                if (CheckCollisionPointRec(mousePos, btnPlus) && bet + 10 <= p.credit) bet += 10;
                if (CheckCollisionPointRec(mousePos, btnBack)) return;
                
                if (CheckCollisionPointRec(mousePos, btnDeal) && p.credit >= bet) {
                    p.credit -= bet;
                    game.randbingo(game.player_paper);
                    game.randbingo(game.enemy_paper);
                    for(int i=0; i<5; i++) for(int j=0; j<5; j++) {
                        game.player_score[i][j] = false;
                        game.enemy_score[i][j] = false;
                    }
                    game.win_type = rand() % 3;
                    ballPool.clear();
                    for(int i=1; i<=99; i++) ballPool.push_back(i);
                    
                    int n = ballPool.size();
                    for (int i = n - 1; i > 0; i--) {
                        int j = rand() % (i + 1);
                        swap(ballPool[i], ballPool[j]);
                    }
                    
                    isPlaying = true;
                    msgColor = RAYWHITE;
                    sysMsg = (game.win_type == 0) ? "MODE: ROW WIN" : (game.win_type == 1) ? "MODE: CROSS WIN" : "MODE: COLUMN WIN";
                }
            }
        } else if (isPlaying) {
            if (isClick && CheckCollisionPointRec(mousePos, btnDraw)) {
                if (!ballPool.empty()) {
                    lastBall = ballPool.back(); ballPool.pop_back();
                    game.checkMatch(game.player_paper, game.player_score, lastBall);
                    game.checkMatch(game.enemy_paper, game.enemy_score, lastBall);

                    bool pWin = game.checkWin(game.player_score, game.win_type);
                    bool eWin = game.checkWin(game.enemy_score, game.win_type);

                    if (pWin || eWin) {
                        isPlaying = false; gameOver = true;
                        if (pWin && !eWin) { p.credit += bet * 2; p.win_count++; sysMsg = "YOU BINGO! WINNER!"; msgColor = GREEN; }
                        else if (eWin && !pWin) { p.loss_count++; sysMsg = "ENEMY BINGO! YOU LOSE."; msgColor = RED; }
                        else { p.credit += bet; sysMsg = "TIE BINGO!"; msgColor = YELLOW; }
                    }
                }
            }
        } else {
            if (isClick && CheckCollisionPointRec(mousePos, btnDeal)) { gameOver = false; sysMsg = "Adjust Bet and Deal!"; msgColor = RAYWHITE; }
            if (isClick && CheckCollisionPointRec(mousePos, btnBack)) return;
        }

        BeginDrawing();
        ClearBackground(Color{ 20, 40, 80, 255 }); 

        DrawText("BINGO BONANZA", (screenWidth - MeasureText("BINGO BONANZA", 40)) / 2, 20, 40, GOLD);
        DrawText(TextFormat("Credits: $%.2f", p.credit), 1050, 30, 25, RAYWHITE);

        if (isPlaying || gameOver) {
            DrawText("YOUR TABLE", 200, 80, 20, SKYBLUE);
            for(int i=0; i<5; i++) {
                for(int j=0; j<5; j++) {
                    Rectangle r = { (float)150 + (j*75), (float)110 + (i*75), 70, 70 };
                    DrawRectangleRec(r, game.player_score[i][j] ? ORANGE : RAYWHITE);
                    if(game.player_paper[i][j] != 0)
                        DrawText(TextFormat("%d", game.player_paper[i][j]), r.x + 35 - MeasureText(TextFormat("%d", game.player_paper[i][j]), 25)/2, r.y + 22, 25, DARKBLUE);
                    else DrawText("F", r.x + 25, r.y + 25, 20, RED);
                }
            }
            DrawText("ENEMY STATUS", 650, 80, 20, RED);
            for(int i=0; i<5; i++) {
                for(int j=0; j<5; j++) {
                    Rectangle r = { (float)650 + (j*45), (float)110 + (i*45), 40, 40 };
                    DrawRectangleRec(r, game.enemy_score[i][j] ? MAROON : GRAY);
                }
            }
            DrawCircle(1050, 250, 70, RAYWHITE);
            if(lastBall > 0) DrawText(TextFormat("%d", lastBall), 1050 - MeasureText(TextFormat("%d", lastBall), 50)/2, 225, 50, BLACK);
        }

        // --- จุดที่แก้ไข: จัดวางข้อความและปุ่มไม่ให้ทับกัน ---
        
        // 1. วาดข้อความแจ้งเตือน (sysMsg) ให้อยู่เหนือแถบ Bet
        DrawText(sysMsg.c_str(), centerX - (MeasureText(sysMsg.c_str(), 25) / 2.0f), 460, 25, msgColor);

        if (!isPlaying && !gameOver) {
            // 2. วาดปุ่มลบ (-)
            DrawRectangleRec(btnMinus, CheckCollisionPointRec(mousePos, btnMinus) ? LIGHTGRAY : GRAY); 
            DrawText("-", btnMinus.x + 18, btnMinus.y + 8, 35, BLACK);
            
            // 3. วาดตัวเลข BET ไว้กึ่งกลางเป๊ะ
            string betTxt = TextFormat("BET: %d", bet);
            DrawText(betTxt.c_str(), centerX - (MeasureText(betTxt.c_str(), 30) / 2.0f), 530, 30, WHITE);
            
            // 4. วาดปุ่มเพิ่ม (+)
            DrawRectangleRec(btnPlus, CheckCollisionPointRec(mousePos, btnPlus) ? LIGHTGRAY : GRAY); 
            DrawText("+", btnPlus.x + 14, btnPlus.y + 8, 35, BLACK);
            
            // 5. ปุ่ม DEAL (พิกัด Y=600)
            DrawRectangleRec(btnDeal, GOLD); 
            DrawText("DEAL CARD", btnDeal.x + 35, btnDeal.y + 18, 25, BLACK);
        } else if (isPlaying) {
            DrawRectangleRec(btnDraw, SKYBLUE); 
            DrawText("DRAW BALL", 985, 468, 22, BLACK);
        } else if (gameOver) {
            DrawRectangleRec(btnDeal, GOLD); 
            DrawText("PLAY AGAIN", btnDeal.x + 35, btnDeal.y + 18, 25, BLACK);
        }

        DrawRectangleRec(btnBack, MAROON); 
        DrawText("BACK", 50, 40, 20, WHITE);
        
        EndDrawing();
    }
}

#endif