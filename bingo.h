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
    int player_paper[5][5];  // เก็บตัวเลขบนกระดานของผู้เล่น (5x5)
    int enemy_paper[5][5];   // เก็บตัวเลขบนกระดานของ enemy (5x5)
    bool player_score[5][5]; // เก็บสถานะว่าช่องไหนถูกวงแล้วบ้าง
    bool enemy_score[5][5];  // เก็บสถานะการวงของ enemy
    int win_type;            // รูปแบบการชนะที่สุ่มได้ (0: แนวนอน, 1: แนวทแยง, 2: แนวตั้ง)

    // สุ่มตัวเลข 1-99 ใส่กระดาน 5x5 โดยไม่ให้ซ้ำกัน
    void randbingo(int table[5][5]) {
        vector<int> nums;
        for(int i=1; i<=99; i++) nums.push_back(i); // ใส่เลข 1-99 ลงในถุงสุ่ม
        
        // สับตำแหน่งข้อมูลใน Vector ให้กระจายแบบสุ่ม
        int n = nums.size();
        for (int i = n - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            swap(nums[i], nums[j]);
        }

        // นำเลขที่สุ่มเสร็จแล้ว 25 ตัวแรก มาวางลงในตาราง 5x5
        for(int i=0; i<5; i++) {
            for(int j=0; j<5; j++) table[i][j] = nums[i * 5 + j];
        }
        table[2][2] = 0; //กำหนดช่องกลางให้เป็น 0 (ช่องฟรี)
    }

    // [ฟังก์ชัน] ตรวจสอบว่าเลขที่จั่วได้ ตรงกับเลขในตารางหรือไม่
    void checkMatch(int table[5][5], bool score[5][5], int drawnNum) {
        for(int i=0; i<5; i++) {
            for(int j=0; j<5; j++) {
                if(table[i][j] == drawnNum) score[i][j] = true; // ถ้าเลขตรงกัน ให้บันทึกสถานะเป็น true
            }
        }
        score[2][2] = true; // ล็อกช่อง FREE (จุดกลาง) ให้ถูกวงไว้ตลอดเวลา
    }

    //ตรวจสอบเงื่อนไขการชนะ
    bool checkWin(bool s[5][5], int type) {
        if(type == 0) { // โหมดชนะด้วยแถวนอน (Row Win)
            for(int i=0; i<5; i++) if(s[i][0] && s[i][1] && s[i][2] && s[i][3] && s[i][4]) return true;
        } else if(type == 1) { // โหมดชนะด้วยแนวทแยง (Diagonal Win)
            if(s[0][0] && s[1][1] && s[2][2] && s[3][3] && s[4][4]) return true; // ทแยงซ้ายไปขวา
            if(s[0][4] && s[1][3] && s[2][2] && s[3][1] && s[4][0]) return true; // ทแยงขวาไปซ้าย
        } else if(type == 2) { // โหมดชนะด้วยแนวตั้ง (Column Win)
            for(int i=0; i<5; i++) if(s[0][i] && s[1][i] && s[2][i] && s[3][i] && s[4][i]) return true;
        }
        return false;
    }
};

// ส่วนจัดการหน้าจอและการเล่น
void playBingoUI(Player &p) {
    // ตั้งค่าตำแหน่งหน้าจอ
    const int screenWidth = 1280;
    const int screenHeight = 720;
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f; 

    // โหลดรูปภาพวิธีเล่น
    Texture2D howToPlayImg = LoadTexture("howtoplay/bg.png");

    BingoGame game;
    int bet = 0; 
    string betInput = "50"; 
    bool betBoxActive = false; // สถานะกล่องรับเงินเดิมพัน

    // gameState: 0=เลือกการ์ด, 1=กำลังเล่น, 2=จบเกม, 3=หน้าสอนเล่น
    int gameState = 0; 
    int previousState = 0;  // ตัวแปรจำว่าก่อนเปิดคู่มือ ผู้เล่นอยู่หน้าไหน

    int lastBall = 0;       // เลขลูกบอลล่าสุดที่จั่วได้
    vector<int> ballPool;   // ถุงเก็บลูกบอลเลข 1-99 สำหรับเริ่มจั่ว
    vector<int> drawnBalls; // ประวัติเลขที่จั่วออกมาแล้วทั้งหมด
    string sysMsg = "Adjust Bet & Choose your Card!";
    Color msgColor = RAYWHITE;

    int card1[5][5], card2[5][5]; // กระดานทางเลือกให้ผู้เล่นเลือกก่อนเริ่ม
    game.randbingo(card1);
    game.randbingo(card2);
    int selectedCard = 1; 

    //การคำนวณตำแหน่ง
    int boxSize = 60, spacing = 6;
    int gridW = (boxSize * 5) + (spacing * 4); // ความกว้างรวมของตาราง
    int c1X = 100, cY = 160; 
    int c2X = screenWidth - 100 - gridW; 

    // พิกัดปุ่มต่างๆ ในหน้าจอ
    Rectangle btnSelect1 = { (float)c1X + (gridW/2.0f) - 100, (float)cY + gridW + 30, 200, 45 };
    Rectangle btnSelect2 = { (float)c2X + (gridW/2.0f) - 100, (float)cY + gridW + 30, 200, 45 };
    Rectangle btnReroll  = { centerX - 120, 230, 240, 50 }; 
    Rectangle boxBet     = { centerX - 100, 360, 200, 50 }; 
    Rectangle btnDeal    = { centerX - 120, 470, 240, 60 }; 
    Rectangle btnDraw    = { centerX - 120, 460, 240, 60 }; 
    Rectangle btnBack    = { 30, 30, 100, 40 };
    Rectangle btnHowToPlay = { 30, 80, 150, 40 }; 

    int frameDelay = 0; // กันบัคคลิกเบิ้ลขณะเปลี่ยนหน้าจอ

    while (!WindowShouldClose()) {
        frameDelay++; 
        Vector2 mousePos = GetMousePosition();
        bool isClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && (frameDelay > 10);

        if (isClick) {
            if (CheckCollisionPointRec(mousePos, btnBack)) {
                if (gameState == 3) {
                    // ถ้าอยู่หน้าสอนเล่น พอกด Back ให้กลับไปหน้าเดิม
                    gameState = previousState;
                    frameDelay = 0;
                } else {
                    // ถ้าอยู่หน้าอื่นๆ (0, 1, 2) แล้วกด Back  = อยากออกเกมบิงโกไปmain
                    UnloadTexture(howToPlayImg);
                    return; 
                }
            }
            if (CheckCollisionPointRec(mousePos, btnHowToPlay) && gameState != 3) {
                // กดปุ่ม How to Play สถานะแล้วเข้าสู่หน้า 3
                previousState = gameState;
                gameState = 3;
                frameDelay = 0;
            }
        }

        //หน้า bet
        if (gameState == 0) { // เลือกกระดานและวางเงิน
            if (isClick) {
                if (CheckCollisionPointRec(mousePos, btnSelect1)) selectedCard = 1;
                if (CheckCollisionPointRec(mousePos, btnSelect2)) selectedCard = 2;
                if (CheckCollisionPointRec(mousePos, btnReroll)) {
                    game.randbingo(card1); game.randbingo(card2);
                    sysMsg = "Cards randomized!"; msgColor = SKYBLUE;
                }
                if (CheckCollisionPointRec(mousePos, boxBet)) betBoxActive = true; else betBoxActive = false;
                
                if (CheckCollisionPointRec(mousePos, btnDeal)) {
                    bet = atoi(betInput.c_str()); 
                    if (p.credit >= bet && bet > 0) {
                        p.credit -= bet; // หักเงินเดิมพัน
                        //คัดลอกเลขจากการ์ดที่เลือกไปใส่กระดานเล่นจริง
                        for(int i=0; i<5; i++) for(int j=0; j<5; j++) 
                            game.player_paper[i][j] = (selectedCard == 1) ? card1[i][j] : card2[i][j];
                        
                        game.randbingo(game.enemy_paper); // สุ่มกระดานให้ คู่แข่ง
                        for(int i=0; i<5; i++) for(int j=0; j<5; j++) {
                            game.player_score[i][j] = false; game.enemy_score[i][j] = false;
                        }
                        game.win_type = rand() % 3; // สุ่มโหมดชนะของรอบนี้
                        ballPool.clear(); drawnBalls.clear();
                        for(int i=1; i<=99; i++) ballPool.push_back(i);
                        
                        // สับลูกบอลในถุงก่อนเริ่มจั่ว
                        for (int i = ballPool.size() - 1; i > 0; i--) swap(ballPool[i], ballPool[rand() % (i + 1)]);
                        
                        gameState = 1; // เริ่มเล่น
                        msgColor = RAYWHITE;
                        sysMsg = (game.win_type == 0) ? "MODE: ROW WIN" : (game.win_type == 1) ? "MODE: CROSS WIN" : "MODE: COLUMN WIN";
                        frameDelay = 0;
                    } else {
                        sysMsg = (bet <= 0) ? "Bet must be > 0!" : "Not enough credits!"; msgColor = RED;
                    }
                }
            }
            // ระบบพิมพ์ตัวเลขเดิมพัน
            if (betBoxActive) {
                int key = GetCharPressed();
                while (key > 0) {
                    if ((key >= '0') && (key <= '9') && betInput.length() < 7) betInput += (char)key;
                    key = GetCharPressed(); 
                }
                if (IsKeyPressed(KEY_BACKSPACE) && betInput.length() > 0) betInput.pop_back();
            }
        } else if (gameState == 1) { // จั่วบอล
            if (isClick && CheckCollisionPointRec(mousePos, btnDraw)) {
                if (!ballPool.empty()) {
                    lastBall = ballPool.back(); ballPool.pop_back(); // จั่วบอลออกจากถุง
                    drawnBalls.push_back(lastBall); // เก็บเข้าประวัติ
                    game.checkMatch(game.player_paper, game.player_score, lastBall);
                    game.checkMatch(game.enemy_paper, game.enemy_score, lastBall);
                    
                    bool pWin = game.checkWin(game.player_score, game.win_type);
                    bool eWin = game.checkWin(game.enemy_score, game.win_type);
                    
                    if (pWin || eWin) { // ตรวจพบคนบิงโก
                        gameState = 2;
                        if (pWin && !eWin) { p.credit += bet * 2; p.win_count++; sysMsg = TextFormat("YOU BINGO! WON $%d", bet*2); msgColor = GREEN; }
                        else if (eWin && !pWin) { p.loss_count++; sysMsg = TextFormat("ENEMY BINGO! LOST $%d", bet); msgColor = RED; }
                        else { p.credit += bet; sysMsg = "TIE BINGO! REFUNDED."; msgColor = YELLOW; }
                    }
                }
            }
        } else if (gameState == 2) { // จบเกม
            if (isClick && CheckCollisionPointRec(mousePos, btnDraw)) { // ปุ่มเล่นใหม่
                gameState = 0; lastBall = 0; drawnBalls.clear();
                game.randbingo(card1); game.randbingo(card2);
                sysMsg = "Adjust Bet & Choose your Card!"; msgColor = RAYWHITE;
                frameDelay = 0;
            }
        } 

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //ส่วนการวาดกราฟิก
        BeginDrawing();
        ClearBackground(Color{ 10, 20, 45, 255 });//พื้นหลัง

        if (gameState == 3) { // วาดหน้าวิธีเล่น
            if (howToPlayImg.id != 0) DrawTexture(howToPlayImg, 0, 0, WHITE);
            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON); 
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);
        } else {
            // ชื่อเกมและเครดิต
            DrawText("BINGO BONANZA", centerX - MeasureText("BINGO BONANZA", 45)/2, 20, 45, GOLD);
            DrawText(TextFormat("Credits: $%.2f", p.credit), 1050, 30, 25, GOLD);
            
            if (gameState == 0) { // วาด UI ช่วงเลือกกระดาน
                DrawText(sysMsg.c_str(), centerX - (MeasureText(sysMsg.c_str(), 25) / 2.0f), 100, 25, msgColor);

                // แผงควบคุมตรงกลาง
                DrawRectangle(centerX - 160, 210, 320, 340, Color{0, 0, 0, 150}); 
                DrawRectangleRec(btnReroll, CheckCollisionPointRec(mousePos, btnReroll) ? SKYBLUE : BLUE);
                DrawText("REROLL CARDS", btnReroll.x + 45, btnReroll.y + 15, 20, WHITE);
                DrawText("BET AMOUNT", centerX - 60, boxBet.y - 25, 20, RAYWHITE);
                DrawRectangleRec(boxBet, betBoxActive ? RAYWHITE : LIGHTGRAY);
                DrawRectangleLinesEx(boxBet, 2, betBoxActive ? GOLD : BLACK);
                DrawText(betInput.c_str(), boxBet.x + 15, boxBet.y + 10, 30, BLACK);
                DrawRectangleRec(btnDeal, RED);
                DrawText("CONFIRM CARD", btnDeal.x + 40, btnDeal.y + 20, 22, GOLD);

                // วาด Card 1 & 2
                for(int i=0; i<5; i++) {
                    for(int j=0; j<5; j++) {
                        // Card 1
                        int x1 = c1X + j*(boxSize+spacing); int y1 = cY + i*(boxSize+spacing);
                        DrawRectangle(x1, y1, boxSize, boxSize, RAYWHITE);
                        if(card1[i][j] != 0) DrawText(to_string(card1[i][j]).c_str(), x1+15, y1+15, 30, DARKBLUE);
                        else { DrawRectangle(x1+2, y1+2, boxSize-4, boxSize-4, GOLD); DrawText("FREE", x1+8, y1+22, 18, BLACK); }
                        // Card 2
                        int x2 = c2X + j*(boxSize+spacing); int y2 = cY + i*(boxSize+spacing);
                        DrawRectangle(x2, y2, boxSize, boxSize, RAYWHITE);
                        if(card2[i][j] != 0) DrawText(to_string(card2[i][j]).c_str(), x2+15, y2+15, 30, DARKBLUE);
                        else { DrawRectangle(x2+2, y2+2, boxSize-4, boxSize-4, GOLD); DrawText("FREE", x2+8, y2+22, 18, BLACK); }
                    }
                }
                // ไฮไลท์การ์ดที่ถูกเลือก
                if (selectedCard == 1) DrawRectangleLinesEx({(float)c1X-6, (float)cY-6, (float)gridW+12, (float)gridW+12}, 6, LIME);
                if (selectedCard == 2) DrawRectangleLinesEx({(float)c2X-6, (float)cY-6, (float)gridW+12, (float)gridW+12}, 6, LIME);
                DrawRectangleRec(btnSelect1, (selectedCard == 1) ? LIME : GRAY);
                DrawText("SELECT CARD 1", btnSelect1.x + 30, btnSelect1.y + 12, 20, BLACK);
                DrawRectangleRec(btnSelect2, (selectedCard == 2) ? LIME : GRAY);
                DrawText("SELECT CARD 2", btnSelect2.x + 30, btnSelect2.y + 12, 20, BLACK);

            } else if (gameState == 1 || gameState == 2) { // วาด UI ช่วงเล่นจริง
                DrawText(sysMsg.c_str(), centerX - MeasureText(sysMsg.c_str(), 30)/2, 100, 30, msgColor);
                
                // วาดกระดานผู้เล่นและ enemy (Highlight สีเมื่อถูกวง)
                for(int i=0; i<5; i++) {
                    for(int j=0; j<5; j++) {
                        // Player Table
                        int x = c1X + j*(boxSize+spacing); int y = cY + i*(boxSize+spacing);
                        DrawRectangle(x, y, boxSize, boxSize, game.player_score[i][j] ? ORANGE : RAYWHITE);
                        if(game.player_paper[i][j] != 0) DrawText(to_string(game.player_paper[i][j]).c_str(), x+15, y+15, 30, DARKBLUE);
                        else DrawText("FREE", x+2, y+20, 20, WHITE);
                        // Enemy Status Table
                        int xe = c2X + j*(boxSize+spacing); int ye = cY + i*(boxSize+spacing);
                        DrawRectangle(xe, ye, boxSize, boxSize, game.enemy_score[i][j] ? MAROON : LIGHTGRAY);
                        if(game.enemy_paper[i][j] != 0) DrawText(to_string(game.enemy_paper[i][j]).c_str(), xe+15, ye+15, 30, DARKGRAY);
                        else DrawText("FREE", xe+2, ye+20, 20, LIGHTGRAY);
                    }
                }

                // วาดลูกบอลล่าสุดที่จั่วได้ (วงกลมกึ่งกลาง)
                DrawCircle(centerX, 310, 75, RAYWHITE); 
                DrawCircleLines(centerX, 310, 75, GOLD);
                if(lastBall > 0) DrawText(to_string(lastBall).c_str(), centerX - MeasureText(to_string(lastBall).c_str(), 70)/2, 275, 70, BLACK);

                // ปุ่มจั่วบอล / เล่นใหม่
                if (gameState == 1) { DrawRectangleRec(btnDraw, SKYBLUE); DrawText("DRAW BALL", btnDraw.x+45, btnDraw.y+15, 30, BLACK); }
                else { DrawRectangleRec(btnDraw, RED); DrawText("PLAY AGAIN", btnDraw.x+55, btnDraw.y+18, 25, GOLD); }

                // History Bar: แสดงลูกบอล 20 ลูกล่าสุดที่ออกไปแล้ว
                DrawRectangle(0, screenHeight - 100, screenWidth, 100, Color{5, 10, 25, 255}); 
                DrawText("CALLED BALLS:", 20, screenHeight - 60, 18, LIGHTGRAY);
                int startIdx = max(0, (int)drawnBalls.size() - 20); int drawX = 170;
                for (int i = startIdx; i < drawnBalls.size(); i++) {
                    DrawCircle(drawX, screenHeight - 50, 22, (i == drawnBalls.size() - 1) ? GOLD : RAYWHITE);
                    DrawText(to_string(drawnBalls[i]).c_str(), drawX - 10, screenHeight - 59, 18, BLACK);
                    drawX += 50; 
                }
            }

            // แสดงปุ่ม BACK ทุก State (ยกเว้น 3)
            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON); 
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);
            
            // แสดงปุ่ม HOW TO PLAY ทุก State (ยกเว้น 3)
            DrawRectangleRec(btnHowToPlay, CheckCollisionPointRec(mousePos, btnHowToPlay) ? GRAY : DARKGRAY);
            DrawText("HOW TO PLAY", btnHowToPlay.x + 15, btnHowToPlay.y + 11, 18, RAYWHITE);
        }
        EndDrawing();
    }
    UnloadTexture(howToPlayImg);
}

#endif