#ifndef HILO_UI_H //ใช้ป้องกันการ include ซ้ำ
#define HILO_UI_H

#include "raylib.h"
#include "Player.h"
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

//ฟังก์ชันสุ่มตัวเลขลูกเต๋า 1 ลูก (คืนค่าเลข 1 ถึง 6)
inline int rollDiceLogic() { return rand() % 6 + 1; }

// วาดรูปทรงลูกเต๋า
static void DrawDiceUI(float x, float y, float size, int value) {
    // วาดพื้นหลังสี่เหลี่ยมสีขาว
    DrawRectangle(x, y, size, size, RAYWHITE);
    
    // พิกัดจุดบนหน้าเต๋า
    float d = size / 4.0f;  // ระยะห่างจากขอบ
    float c = size / 2.0f;  // จุดกึ่งกลาง
    float r = size / 10.0f; // รัศมีของจุดเต๋า
    Color dotC = BLACK;     // สีจุดปกติ

    // ตรวจสอบค่าลูกเต๋าเพื่อวาดจุดในตำแหน่งที่ถูกต้อง (1-6)
    if (value == 1) { 
        DrawCircle(x+c, y+c, r*1.5f, RED); // เลข 1: จุดแดงใหญ่ตรงกลาง
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
    // ตั้งค่าหน้าจอ
    const int screenWidth = 1280;
    const int screenHeight = 720;
    float centerX = screenWidth / 2.0f; // จุดกึ่งกลาง x
    float centerY = screenHeight / 2.0f; // จุดกึ่งกลาง y

    // โหลดรูปภาพวิธีเล่น
    Texture2D htpImg = LoadTexture("howtoplay/hl.png");

    //ลิ้งกับ UI
    int choice = 1;       // โหมดการทาย (1: สูง/ต่ำ, 2: ตอง, 3: ผลรวม)
    int hlChoice = 1;     // เลือกทาย ต่ำ(1) หรือ สูง(2)
    int guessTriple = 1;  // เลือกเลขตองที่จะทาย (1-6)
    int guessSum = 10;    // เลือกเลขผลรวมที่จะทาย (3-18)
    int luckyNumber = 1;  // เลขนำโชค
    int winStreak = 0;    // สถิติการชนะต่อเนื่อง
    
    int bet = 0;
    string betInput = "50"; 
    bool betBoxActive = false; // สถานะการพิมพ์ในกล่องเดิมพัน ไฮไลท์ตอนกดจะเรืองเเเสง

    int dice[3] = {1, 1, 1}; // เก็บผลลัพธ์ลูกเต๋า 3 ลูก
    bool isRolling = false;  // สถานะว่ากำลังเขย่าลูกเต๋าอยู่หรือไม่
    float rollTimer = 0.0f;  // ตัวนับเวลาสำหรับ animation การเขย่า
    string sysMsg = "Adjust Bet, select Mode/Lucky No, then ROLL!"; //ของ UI ใต้กล่อง
    Color msgColor = RAYWHITE;

    int gameState = 0; // 0=หน้าหลัก/เดิมพัน, 3=หน้าวิธีเล่น 1ระหว่างประมวลเล่น 2สรุปผล

    //กำหนดตำแหน่งปุ่ม สร้างค่าไว้เฉยๆ
    Rectangle btnBack   = { 30, 30, 100, 40 }; //ปุ่มกลับ
    Rectangle btnHTP    = { 30, 80, 150, 40 }; //ปุ่มเปิดคู่มือ
    Rectangle boxBet    = { centerX - 260, 520, 200, 45 }; // กล่องเดิมพัน
    Rectangle btnRoll   = { centerX - 120, 620, 240, 60 }; // ปุ่มทอยเต๋า

    int frameDelay = 0; // ป้องกันบัคจากการคลิกเมาส์เร็วเกิน ไว้กันดีเลย์

    while (!WindowShouldClose()){ //ไว้ออกลูป
        frameDelay++; 
        Vector2 mousePos = GetMousePosition(); //ไว้ขยับเม้าเเกน x y
        // เช็คการคลิกเมาส์ซ้ายพร้อมหน่วงเวลา
        bool isClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && (frameDelay > 10); //สร้างจาก function ของ raylib

        // พิกัดปุ่มเลือกโหมดการเล่น
        Rectangle rMode1 = { 380, 280, 180, 45 }; 
        Rectangle rMode2 = { 580, 280, 180, 45 }; 
        Rectangle rMode3 = { 780, 280, 180, 45 }; 

        //ส่วนของการคำนวณ
        if (gameState == 0) {
            if (!isRolling) {
                if (isClick) { //เอา isclick มาใช้
                    // ปุ่มย้อนกลับและวิธีเล่น
                    if (CheckCollisionPointRec(mousePos, btnBack)) { UnloadTexture(htpImg); return; }
                    if (CheckCollisionPointRec(mousePos, btnHTP)) gameState = 3; //หน้่าวิธีเล่น
                    
                    // เปิด/ปิดการพิมพ์ในกล่อง Bet
                    if (CheckCollisionPointRec(mousePos, boxBet)) betBoxActive = true; else betBoxActive = false;

                    // เปลี่ยนโหมดการเล่น (High-Low / Triple / Sum)
                    if (CheckCollisionPointRec(mousePos, rMode1)) choice = 1;
                    if (CheckCollisionPointRec(mousePos, rMode2)) choice = 2;
                    if (CheckCollisionPointRec(mousePos, rMode3)) choice = 3;

                    // เลือกเงื่อนไขย่อยตามโหมดที่เลือก
                    if (choice == 1) { // โหมด สูง-ต่ำ
                        if (CheckCollisionPointRec(mousePos, { 380, 345, 150, 40 })) hlChoice = 1; // LOW
                        if (CheckCollisionPointRec(mousePos, { 550, 345, 150, 40 })) hlChoice = 2; // HIGH
                    } else if (choice == 2) { // โหมด เลขตอง
                        for(int i=1; i<=6; i++) 
                            if (CheckCollisionPointRec(mousePos, { 380.0f + ((i-1)*70), 345, 60, 40 })) guessTriple = i;
                    } else if (choice == 3) { // โหมด ทายผลรวม
                        if (CheckCollisionPointRec(mousePos, { 380, 345, 60, 40 }) && guessSum > 3) guessSum--;
                        if (CheckCollisionPointRec(mousePos, { 600, 345, 60, 40 }) && guessSum < 18) guessSum++;
                    }

                    // เลือกเลขนำโชค (ออกกี่ลูก ได้เงินคืนลูกละ 1 เท่า)
                    for(int i=1; i<=6; i++)
                        if (CheckCollisionPointRec(mousePos, { 380.0f + ((i-1)*70), 430, 60, 40 })) luckyNumber = i;

                    // เมื่อกดปุ่ม ROLL (เริ่มทอยเต๋า)
                    if (CheckCollisionPointRec(mousePos, btnRoll)) {
                        bet = atoi(betInput.c_str()); // แปลง String เป็นตัวเลข
                        if (p.credit >= bet && bet > 0) {
                            p.credit -= bet; // หักเงินเดิมพัน
                            isRolling = true; betBoxActive = false; rollTimer = 0.0f;
                            sysMsg = "Shaking..."; msgColor = RAYWHITE;
                        } else { 
                            sysMsg = (bet <= 0) ? "Bet must be > 0!" : "Not enough credits!"; 
                            msgColor = RED; 
                        }
                    }
                }
                // จัดการการพิมพ์ตัวเลขเดิมพันผ่านคีย์บอร์ด
                if (betBoxActive) {
                    int key = GetCharPressed();
                    while (key > 0) { //วนลูปรับตัวอักษรทั้งหมด
                        if ((key >= '0') && (key <= '9') && betInput.length() < 7) betInput += (char)key; //ตรวจว่าเป็นตัวเลขหรือไม่พิมได้เฉพาะเลขไม่เกิน 7 หลัก
                        key = GetCharPressed(); 
                    }
                    if (IsKeyPressed(KEY_BACKSPACE) && betInput.length() > 0) betInput.pop_back(); // ไว้ลบ
                }
            } else {
                //จำลองการเขย่าลูกเต๋าโดยสุ่มเลขรัวๆ1.5 วินาที
                rollTimer += GetFrameTime(); //ของ rylib
                dice[0] = rollDiceLogic(); dice[1] = rollDiceLogic(); dice[2] = rollDiceLogic();
                
                if (rollTimer >= 1.5f) { // เมื่อเขย่าเสร็จ
                    isRolling = false;
                    int sum = dice[0] + dice[1] + dice[2];
                    bool win = false; int reward = 0;

                    // ตรวจสอบรางวัลตามโหมดที่เลือก
                    if (choice == 1) { // สูง/ต่ำ
                        if (!(dice[0] == dice[1] && dice[1] == dice[2])) { // คำนวณสูงต่ำ
                            if (hlChoice == 1 && sum >= 4 && sum <= 10) { win = true; reward = bet * 2; }
                            else if (hlChoice == 2 && sum >= 11 && sum <= 17) { win = true; reward = bet * 2; }
                        }
                    } else if (choice == 2) { // ตอง
                        if (dice[0] == dice[1] && dice[1] == dice[2] && dice[0] == guessTriple) { win = true; reward = bet * 5; }
                    } else if (choice == 3) { // ผลรวม
                        if (sum == guessSum) { win = true; reward = bet * 3; }
                    }

                    // เช็คเลขนำโชค (ได้เงินคืนเพิ่ม)
                    int luckyCount = 0;
                    for(int i=0; i<3; i++) if (dice[i] == luckyNumber) luckyCount++;

                    // สรุปผลเงินรางวัลและบันทึกสถิติ
                    if (win) {
                        p.credit += reward; winStreak++; p.win_count++;
                        sysMsg = TextFormat("WIN! +$%d", reward + (luckyCount * bet)); msgColor = GREEN;
                    } else {
                        winStreak = 0; p.loss_count++;
                        sysMsg = (luckyCount > 0) ? TextFormat("Lucky No gave +$%d", luckyCount * bet) : "LOSE!"; 
                        msgColor = (luckyCount > 0) ? YELLOW : RED;
                    }
                    p.credit += (luckyCount * bet); // คืนเงินจากเลขนำโชค
                }
            }
        } else if (gameState == 3) { // หน้าวิธีเล่น
            if (isClick && CheckCollisionPointRec(mousePos, btnBack)) gameState = 0;
        }

        // ส่วนของการวาดผลลัพธ์
        BeginDrawing();
        ClearBackground(Color{ 10, 50, 20, 255 }); // สีเขียว

        if (gameState == 3) { // แสดงภาพวิธีเล่น
            if (htpImg.id != 0) DrawTexture(htpImg, 0, 0, WHITE);
            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON);
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);
        } else {
            //หัวข้อเกม
            DrawText("HI-LO CASINO", centerX - MeasureText("HI-LO CASINO", 40)/2, 20, 40, GOLD);
            
            //แสดงข้อมูลผู้เล่น
            DrawText(TextFormat("Credits: $%.2f", p.credit), 1050, 30, 22, WHITE);
            DrawText(TextFormat("Streak: %d", winStreak), 1050, 55, 22, GOLD);

            //วาดลูกเต๋า 3 ลูก
            int dSize = 100; float dStartX = centerX - (dSize * 1.5f) - 20; 
            for (int i = 0; i < 3; i++) DrawDiceUI(dStartX + (i * 140), 110, dSize, dice[i]);
            
            //ข้อความแจ้งเตือนระบบ
            DrawText(sysMsg.c_str(), centerX - MeasureText(sysMsg.c_str(), 25)/2, 230, 25, msgColor);
            
            //พื้นหลังแผงควบคุม
            DrawRectangle(140, 270, 1000, 320, Color{0, 0, 0, 100});

            //เลือกโหมด (Mode Selection)
            DrawText("1. SELECT MODE:", 180, 290, 20, WHITE);
            DrawRectangleRec(rMode1, (choice == 1) ? GOLD : GRAY); DrawText("High/Low", rMode1.x+45, rMode1.y+12, 20, BLACK);
            DrawRectangleRec(rMode2, (choice == 2) ? GOLD : GRAY); DrawText("Triple", rMode2.x+60, rMode2.y+12, 20, BLACK);
            DrawRectangleRec(rMode3, (choice == 3) ? GOLD : GRAY); DrawText("Sum", rMode3.x+70, rMode3.y+12, 20, BLACK);

            //วาดตัวเลือกย่อยของแต่ละโหมด
            if (choice == 1) {
                DrawRectangleRec({ 380, 345, 150, 40 }, (hlChoice == 1) ? ORANGE : GRAY); DrawText("LOW", 430, 355, 20, BLACK);
                DrawRectangleRec({ 550, 345, 150, 40 }, (hlChoice == 2) ? ORANGE : GRAY); DrawText("HIGH", 595, 355, 20, BLACK);
            } else if (choice == 2) {
                for(int i=1; i<=6; i++) {
                    Rectangle r = { 380.0f + ((i-1)*70), 345, 60, 40 };
                    DrawRectangleRec(r, (guessTriple == i) ? ORANGE : GRAY); DrawText(TextFormat("%d", i), r.x+25, r.y+10, 20, BLACK);
                }
            } else if (choice == 3) {
                DrawText(TextFormat("SUM: %d", guessSum), 465, 350, 25, YELLOW);
                DrawRectangleRec({ 380, 345, 60, 40 }, GRAY); DrawText("-", 403, 350, 30, BLACK);
                DrawRectangleRec({ 600, 345, 60, 40 }, GRAY); DrawText("+", 620, 350, 30, BLACK);
            }

            //เลือกเลขนำโชค (Lucky Number)
            DrawText("2. LUCKY NO:", 180, 440, 20, SKYBLUE);
            for(int i=1; i<=6; i++) {
                Rectangle r = { 380.0f + ((i-1)*70), 430, 60, 40 };
                DrawRectangleRec(r, (luckyNumber == i) ? SKYBLUE : GRAY); DrawText(TextFormat("%d", i), r.x+25, r.y+10, 20, BLACK);
            }

            //  กำหนดเงินเดิมพัน (Bet Area)
            DrawText("3. BET AMOUNT:", 180, 532, 20, PINK);
            DrawRectangleRec(boxBet, betBoxActive ? RAYWHITE : LIGHTGRAY);
            // ไฮไลท์สีทองเมื่อกล่อง Bet ถูกเลือก
            DrawRectangleLinesEx(boxBet, 2, betBoxActive ? GOLD : BLACK);
            DrawText(betInput.c_str(), boxBet.x+15, boxBet.y+8, 30, BLACK);

            // ปุ่มทอยเต๋า (ROLL)
            DrawRectangleRec(btnRoll, isRolling ? DARKGRAY : RED);
            DrawRectangleLinesEx(btnRoll, 4, isRolling ? LIGHTGRAY : GOLD);
            DrawText(isRolling ? "SHAKING..." : "ROLL DICE", btnRoll.x+60, btnRoll.y+18, 25, GOLD);

            // ปุ่ม BACK และ วิธีการเล่น
            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON); 
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);
            if (!isRolling) {
                DrawRectangleRec(btnHTP, CheckCollisionPointRec(mousePos, btnHTP) ? GRAY : DARKGRAY);
                DrawText("HOW TO PLAY", btnHTP.x + 15, btnHTP.y + 10, 18, WHITE);
            }
        }
        EndDrawing();
    }

    UnloadTexture(htpImg);
}
#endif