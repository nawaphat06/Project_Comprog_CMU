#ifndef BINGO_H 
#define BINGO_H 

#include "raylib.h"
#include "Player.h"
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>
#include <cstdlib> 

using namespace std;

class BingoGame {
public:
    int player_paper[5][5];
    int enemy_paper[5][5];
    bool player_score[5][5];
    bool enemy_score[5][5];
    int win_type; 

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
        table[2][2] = 0; // FREE SPACE
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
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f; // แก้ไข Error: centerY was not declared

    // --- โหลดรูปภาพ How to play ---
    Texture2D howToPlayImg = LoadTexture("howtoplay/bg.png");

    BingoGame game;
    int bet = 0; 
    string betInput = "50"; 
    bool betBoxActive = false; 

    int gameState = 0; // 0=เริ่มเกม, 1=เล่น, 2=จบเกม, 3=หน้าสอนเล่น
    int lastBall = 0;
    vector<int> ballPool;
    vector<int> drawnBalls; 
    string sysMsg = "Adjust Bet & Choose your Card!";
    Color msgColor = RAYWHITE;

    int card1[5][5];
    int card2[5][5];
    game.randbingo(card1);
    game.randbingo(card2);
    int selectedCard = 1; 

    // ==========================================
    // พิกัด Layout
    // ==========================================
    int boxSize = 60;
    int spacing = 6;
    int gridW = (boxSize * 5) + (spacing * 4); 
    
    int c1X = 100, cY = 160; 
    int c2X = screenWidth - 100 - gridW; 

    Rectangle btnSelect1 = { (float)c1X + (gridW/2.0f) - 100, (float)cY + gridW + 30, 200, 45 };
    Rectangle btnSelect2 = { (float)c2X + (gridW/2.0f) - 100, (float)cY + gridW + 30, 200, 45 };
    
    Rectangle btnReroll  = { centerX - 120, 230, 240, 50 }; 
    Rectangle boxBet     = { centerX - 100, 370, 200, 50 }; 
    Rectangle btnDeal    = { centerX - 120, 480, 240, 60 }; 
    Rectangle btnHowToPlay = { centerX - 90, 550, 180, 40 }; // ปุ่ม How To Play

    Rectangle btnDraw    = { centerX - 120, 460, 240, 60 }; 
    Rectangle btnBack    = { 30, 30, 100, 40 };

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        bool isClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (gameState == 0) {
            if (isClick) {
                if (CheckCollisionPointRec(mousePos, btnBack)) {
                    UnloadTexture(howToPlayImg);
                    return;
                }
                if (CheckCollisionPointRec(mousePos, btnHowToPlay)) gameState = 3; 
                if (CheckCollisionPointRec(mousePos, btnSelect1)) selectedCard = 1;
                if (CheckCollisionPointRec(mousePos, btnSelect2)) selectedCard = 2;
                if (CheckCollisionPointRec(mousePos, btnReroll)) {
                    game.randbingo(card1);
                    game.randbingo(card2);
                    sysMsg = "Cards randomized!";
                    msgColor = SKYBLUE;
                }
                if (CheckCollisionPointRec(mousePos, boxBet)) betBoxActive = true;
                else betBoxActive = false;
                
                if (CheckCollisionPointRec(mousePos, btnDeal)) {
                    bet = atoi(betInput.c_str()); 
                    if (p.credit >= bet && bet > 0) {
                        p.credit -= bet; 
                        for(int i=0; i<5; i++) for(int j=0; j<5; j++) 
                            game.player_paper[i][j] = (selectedCard == 1) ? card1[i][j] : card2[i][j];
                        game.randbingo(game.enemy_paper);
                        for(int i=0; i<5; i++) for(int j=0; j<5; j++) {
                            game.player_score[i][j] = false;
                            game.enemy_score[i][j] = false;
                        }
                        game.win_type = rand() % 3;
                        ballPool.clear(); drawnBalls.clear();
                        for(int i=1; i<=99; i++) ballPool.push_back(i);
                        
                        // Shuffle Deck
                        for (int i = ballPool.size() - 1; i > 0; i--) {
                            int j = rand() % (i + 1);
                            swap(ballPool[i], ballPool[j]);
                        }
                        gameState = 1;
                        msgColor = RAYWHITE;
                        sysMsg = (game.win_type == 0) ? "MODE: ROW WIN" : (game.win_type == 1) ? "MODE: CROSS WIN" : "MODE: COLUMN WIN";
                    } else {
                        sysMsg = (bet <= 0) ? "Bet must be > 0!" : "Not enough credits!";
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
        } else if (gameState == 1) {
            if (isClick && CheckCollisionPointRec(mousePos, btnDraw)) {
                if (!ballPool.empty()) {
                    lastBall = ballPool.back(); ballPool.pop_back();
                    drawnBalls.push_back(lastBall);
                    game.checkMatch(game.player_paper, game.player_score, lastBall);
                    game.checkMatch(game.enemy_paper, game.enemy_score, lastBall);
                    
                    bool pWin = game.checkWin(game.player_score, game.win_type);
                    bool eWin = game.checkWin(game.enemy_score, game.win_type);
                    
                    if (pWin || eWin) {
                        gameState = 2;
                        if (pWin && !eWin) { p.credit += bet * 2; p.win_count++; sysMsg = TextFormat("YOU BINGO! WON $%d", bet*2); msgColor = GREEN; }
                        else if (eWin && !pWin) { p.loss_count++; sysMsg = TextFormat("ENEMY BINGO! LOST $%d", bet); msgColor = RED; }
                        else { p.credit += bet; sysMsg = "TIE BINGO! REFUNDED."; msgColor = YELLOW; }
                    }
                }
            }
        } else if (gameState == 2) {
            if (isClick) {
                if (CheckCollisionPointRec(mousePos, btnDraw)) { 
                    gameState = 0; lastBall = 0; drawnBalls.clear();
                    game.randbingo(card1); game.randbingo(card2);
                    sysMsg = "Adjust Bet & Choose your Card!"; msgColor = RAYWHITE;
                }
                if (CheckCollisionPointRec(mousePos, btnBack)) {
                    UnloadTexture(howToPlayImg);
                    return;
                }
            }
        } else if (gameState == 3) {
            if (isClick && CheckCollisionPointRec(mousePos, btnBack)) gameState = 0;
        }

        BeginDrawing();
        ClearBackground(Color{ 10, 20, 45, 255 }); 

        if (gameState == 3) {
            if (howToPlayImg.id != 0) DrawTexture(howToPlayImg, 0, 0, WHITE);
            else DrawText("IMAGE NOT FOUND", centerX - MeasureText("IMAGE NOT FOUND", 40)/2, centerY, 40, RED);
            
            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON); 
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);
        } else {
            DrawText("BINGO BONANZA", (screenWidth - MeasureText("BINGO BONANZA", 45)) / 2, 20, 45, GOLD);
            DrawText(TextFormat("Credits: $%.2f", p.credit), 1050, 30, 25, GOLD);
            
            if (gameState == 0) {
                DrawText(sysMsg.c_str(), centerX - (MeasureText(sysMsg.c_str(), 25) / 2.0f), 100, 25, msgColor);

                // ส่วนแผงควบคุมตรงกลาง
                DrawRectangle(centerX - 160, 220, 320, 380, Color{0, 0, 0, 150}); 
                
                DrawRectangleRec(btnReroll, CheckCollisionPointRec(mousePos, btnReroll) ? SKYBLUE : BLUE);
                DrawText("REROLL CARDS", btnReroll.x + (btnReroll.width - MeasureText("REROLL CARDS", 20))/2, btnReroll.y + 15, 20, WHITE);

                DrawText("BET AMOUNT", centerX - MeasureText("BET AMOUNT", 20)/2, boxBet.y - 25, 20, RAYWHITE);
                DrawRectangleRec(boxBet, betBoxActive ? RAYWHITE : LIGHTGRAY);
                DrawRectangleLinesEx(boxBet, 2, betBoxActive ? GOLD : BLACK);
                string dBet = betInput; if(betBoxActive && (int)(GetTime()*2)%2==0) dBet += "_";
                DrawText(dBet.c_str(), boxBet.x + (boxBet.width/2) - (MeasureText(dBet.c_str(), 30)/2), boxBet.y + 10, 30, BLACK);

                DrawRectangleRec(btnDeal, CheckCollisionPointRec(mousePos, btnDeal) ? MAROON : RED);
                DrawText("CONFIRM CARD", btnDeal.x + (btnDeal.width - MeasureText("CONFIRM CARD", 22))/2, btnDeal.y + 20, 22, GOLD);

                Color htpBg = CheckCollisionPointRec(mousePos, btnHowToPlay) ? GRAY : DARKGRAY;
                DrawRectangleRec(btnHowToPlay, htpBg);
                DrawText("HOW TO PLAY", btnHowToPlay.x + (btnHowToPlay.width - MeasureText("HOW TO PLAY", 18))/2, btnHowToPlay.y + 11, 18, RAYWHITE);

                // วาดกระดาน Card 1 & 2
                for(int i=0; i<5; i++) {
                    for(int j=0; j<5; j++) {
                        int x1 = c1X + j*(boxSize+spacing); int y1 = cY + i*(boxSize+spacing);
                        DrawRectangle(x1, y1, boxSize, boxSize, RAYWHITE);
                        if(card1[i][j] != 0) DrawText(to_string(card1[i][j]).c_str(), x1 + (boxSize - MeasureText(to_string(card1[i][j]).c_str(), 30))/2, y1 + 15, 30, DARKBLUE);
                        else { DrawRectangle(x1+2, y1+2, boxSize-4, boxSize-4, GOLD); DrawText("FREE", x1+8, y1+22, 18, BLACK); }
                        
                        int x2 = c2X + j*(boxSize+spacing); int y2 = cY + i*(boxSize+spacing);
                        DrawRectangle(x2, y2, boxSize, boxSize, RAYWHITE);
                        if(card2[i][j] != 0) DrawText(to_string(card2[i][j]).c_str(), x2 + (boxSize - MeasureText(to_string(card2[i][j]).c_str(), 30))/2, y2 + 15, 30, DARKBLUE);
                        else { DrawRectangle(x2+2, y2+2, boxSize-4, boxSize-4, GOLD); DrawText("FREE", x2+8, y2+22, 18, BLACK); }
                    }
                }
                DrawRectangleRec(btnSelect1, (selectedCard == 1) ? LIME : GRAY); DrawText("SELECT CARD 1", btnSelect1.x+30, btnSelect1.y+12, 20, BLACK);
                DrawRectangleRec(btnSelect2, (selectedCard == 2) ? LIME : GRAY); DrawText("SELECT CARD 2", btnSelect2.x+30, btnSelect2.y+12, 20, BLACK);
            }
            
            else if (gameState == 1 || gameState == 2) {
                DrawText(sysMsg.c_str(), centerX - (MeasureText(sysMsg.c_str(), 30) / 2.0f), 100, 30, msgColor);
                
                // Your Table
                DrawText("YOUR TABLE", c1X + (gridW/2) - (MeasureText("YOUR TABLE", 25)/2), cY - 40, 25, SKYBLUE);
                for(int i=0; i<5; i++) for(int j=0; j<5; j++) {
                    int x = c1X + j*(boxSize+spacing); int y = cY + i*(boxSize+spacing);
                    DrawRectangle(x, y, boxSize, boxSize, game.player_score[i][j] ? ORANGE : RAYWHITE);
                    if(game.player_paper[i][j] != 0) DrawText(to_string(game.player_paper[i][j]).c_str(), x + 15, y + 15, 30, DARKBLUE);
                    else DrawText("FREE", x+8, y+22, 18, BLACK);
                }

                // Enemy Status
                DrawText("ENEMY STATUS", c2X + (gridW/2) - (MeasureText("ENEMY STATUS", 25)/2), cY - 40, 25, RED);
                for(int i=0; i<5; i++) for(int j=0; j<5; j++) {
                    int x = c2X + j*(boxSize+spacing); int y = cY + i*(boxSize+spacing);
                    DrawRectangle(x, y, boxSize, boxSize, game.enemy_score[i][j] ? MAROON : LIGHTGRAY);
                    if(game.enemy_paper[i][j] != 0) DrawText(to_string(game.enemy_paper[i][j]).c_str(), x + 15, y + 15, 30, DARKGRAY);
                    else DrawText("FREE", x+8, y+22, 18, BLACK);
                }

                // Last Ball Area
                DrawText("LAST BALL", centerX - (MeasureText("LAST BALL", 20)/2), 200, 20, RAYWHITE);
                DrawCircle(centerX, 310, 75, RAYWHITE); 
                DrawCircleLines(centerX, 310, 75, GOLD);
                if(lastBall > 0) DrawText(to_string(lastBall).c_str(), centerX - MeasureText(to_string(lastBall).c_str(), 70)/2, 275, 70, BLACK);
                else DrawText("READY", centerX - 45, 295, 30, LIGHTGRAY);

                if (gameState == 1) {
                    DrawRectangleRec(btnDraw, SKYBLUE);
                    DrawText("DRAW BALL", btnDraw.x + 45, btnDraw.y + 15, 30, BLACK);
                } else {
                    DrawRectangleRec(btnDraw, GOLD);
                    DrawText("PLAY AGAIN", btnDraw.x + 45, btnDraw.y + 18, 25, BLACK);
                }

                // Called History
                DrawRectangle(0, 620, screenWidth, 100, Color{5, 10, 25, 255}); 
                DrawText("CALLED:", 20, 660, 20, LIGHTGRAY);
                int start = max(0, (int)drawnBalls.size() - 20);
                for (int i = start; i < drawnBalls.size(); i++) {
                    int dx = 140 + (i-start)*52;
                    DrawCircle(dx, 670, 24, (i == drawnBalls.size()-1) ? GOLD : RAYWHITE);
                    DrawText(to_string(drawnBalls[i]).c_str(), dx - MeasureText(to_string(drawnBalls[i]).c_str(), 20)/2, 660, 20, BLACK);
                }
            }

            if (gameState != 1) { 
                DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON); 
                DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);
            }
        }
        EndDrawing();
    }
    UnloadTexture(howToPlayImg);
}

#endif