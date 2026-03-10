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
    string name;   // ชื่อที่จะแสดงผล
    int value;     // อัตราคูณเงินรางวัลเมื่อชนะ
    int weight;    // โอกาสออก
    Color color;   // สีของตัวอักษร
};

// กำหนดสัญลักษณ์ และ %
static vector<SlotSymbol> slotSymbols = {
    {"777", 200, 2, GOLD},
    {"BAR", 100, 6, RED},           
    {"DIA", 30, 8, SKYBLUE},       
    {"CHRY", 15, 10, MAGENTA},     
    {" K ", 7, 14, DARKGRAY}, 
    {" Q ", 5, 15, DARKGRAY}, 
    {" J ", 3, 20, DARKGRAY},  
    {" 10 ", 1 , 25, DARKGRAY}
};

//ฟังก์ชันสุ่มสัญลักษณ์
static SlotSymbol spinSlotSymbol() {
    int totalWeight = 0;
    //รวมน้ำหนักทั้งหมดก่อน
    for (const auto& s : slotSymbols) totalWeight += s.weight;
    
    //สุ่มเลขในช่วงน้ำหนักทั้งหมด
    int rnd = rand() % totalWeight;
    
    //วนลูปหักลบน้ำหนักไปเรื่อยๆ จนกว่าค่าสุ่มจะอยู่ในช่วงของสัญลักษณ์นั้น
    for (const auto& s : slotSymbols) {
        if (rnd < s.weight) return s;
        rnd -= s.weight;
    }
    return slotSymbols.back();//ให้คืนค่าตัวสุดท้าย
}

void playSlotUI(Player &p) {
    //ตั้งค่าตำแหน่งหน้าจอ
    const int screenWidth = 1280;
    const int screenHeight = 720;
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;

    //โหลดรูปภาพสอนเล่น
    Texture2D htpImg = LoadTexture("howtoplay/sl.png");

    int bet = 0;
    string betInput = "10"; 
    bool betBoxActive = false; //สถานะว่ากำลังพิมพ์กล่องเดิมพันอยู่หรือไม่

    //สร้างกระดาน 3x3 เก็บสัญลักษณ์เริ่มต้นเป็นเลข "10"
    vector<vector<SlotSymbol>> board(3, vector<SlotSymbol>(3, slotSymbols[7])); 
    
    //ตาราง 3x3 เพื่อบันทึกว่าช่องไหนบ้างที่ทำเงิน (ไฮไลท์สีเขียว)
    bool winCells[3][3] = { {false, false, false}, {false, false, false}, {false, false, false} };

    // gameState: 0 = เล่นปกติ, 3 = หน้าจอวิธีเล่น
    int gameState = 0; 
    bool isSpinning = false; // สถานะว่ากำลังหมุนอยู่หรือไม่
    float spinTimer = 0.0f;  // ตัวนับเวลาสำหรับ animation ตอนหมุน
    string sysMsg = "Enter Bet and Press SPIN!";
    Color msgColor = RAYWHITE;

    // กำหนดพิกัดปุ่ม
    Rectangle btnBack  = { 30.0f, 30.0f, 100.0f, 40.0f };
    Rectangle btnHTP   = { 30.0f, 80.0f, 150.0f, 40.0f };
    Rectangle boxBet   = { centerX - 100, 585, 200, 45 }; 
    Rectangle btnSpin  = { centerX - 130, 645, 260, 55 };

    int frameDelay = 0; // กันบัคคลิกเบิ้ลระหว่างเปลี่ยนหน้าจอ

    while (!WindowShouldClose()) {
        frameDelay++;
        Vector2 mousePos = GetMousePosition();
        // เช็คการคลิกพร้อมตัวหน่วงเวลา (frameDelay > 10)
        bool isClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && (frameDelay > 10);

        //ส่วนของการประมวลผล
        if (gameState == 0) {
            if (!isSpinning) {
                if (isClick) {
                    // ปุ่มย้อนกลับ
                    if (CheckCollisionPointRec(mousePos, btnBack)) { UnloadTexture(htpImg); return; }
                    // ปุ่มเปิดวิธีเล่น
                    if (CheckCollisionPointRec(mousePos, btnHTP)) gameState = 3;
                    // เช็คคลิกที่กล่อง Bet
                    if (CheckCollisionPointRec(mousePos, boxBet)) betBoxActive = true; else betBoxActive = false;
                    
                    // ปุ่มเริ่มหมุน (SPIN)
                    if (CheckCollisionPointRec(mousePos, btnSpin)) {
                        bet = atoi(betInput.c_str()); 
                        if (p.credit >= bet && bet > 0) {
                            p.credit -= bet; // หักเงินเดิมพัน
                            isSpinning = true;
                            betBoxActive = false; 
                            spinTimer = 0.0f;
                            sysMsg = "Spinning..."; msgColor = RAYWHITE;
                            // ล้างสถานะชนะของรอบเก่า
                            for(int i=0; i<3; i++) for(int j=0; j<3; j++) winCells[i][j] = false;
                        } else {
                            sysMsg = (bet <= 0) ? "Bet must be > 0!" : "Not enough credits!";
                            msgColor = RED;
                        }
                    }
                }
                // ระบบพิมพ์ตัวเลขเดิมพัน ( 0-9 และ Backspace)
                if (betBoxActive) {
                    int key = GetCharPressed();
                    while (key > 0) {
                        if ((key >= '0') && (key <= '9') && betInput.length() < 7) betInput += (char)key;
                        key = GetCharPressed(); 
                    }
                    if (IsKeyPressed(KEY_BACKSPACE) && betInput.length() > 0) betInput.pop_back();
                }
            } else {
                //กำลังหมุนสล็อต
                spinTimer += GetFrameTime();
                //สุ่มสัญลักษณ์ใหม่ทุกช่องรัวๆ เพื่อให้ภาพดูเหมือนกำลังวิ่ง
                for(int i=0; i<3; i++) for(int j=0; j<3; j++) board[i][j] = spinSlotSymbol();

                //เมื่อหมุนครบ 1.5 วินาที ให้หยุดและตรวจรางวัล
                if (spinTimer >= 1.5f) {
                    isSpinning = false;
                    int totalWinRound = 0;
                    bool hasWinLine = false;

                    //ตรวจสอบรางวัลแถวนอน 3 เส้น
                    for (int i = 0; i < 3; i++) {
                        if (board[i][0].name == board[i][1].name && board[i][1].name == board[i][2].name) {
                            totalWinRound += board[i][0].value * bet;
                            hasWinLine = true;
                            winCells[i][0] = winCells[i][1] = winCells[i][2] = true; // มาร์คช่องที่ชนะ
                        }
                    }
                    //ตรวจสอบรางวัลแนวทแยง (ซ้ายบนลงขวาล่าง)
                    if (board[0][0].name == board[1][1].name && board[1][1].name == board[2][2].name) {
                        totalWinRound += board[0][0].value * bet;
                        hasWinLine = true;
                        winCells[0][0] = winCells[1][1] = winCells[2][2] = true;
                    }
                    //ตรวจสอบรางวัลแนวทแยง (ซ้ายล่างขึ้นขวาบน)
                    if (board[2][0].name == board[1][1].name && board[1][1].name == board[0][2].name) {
                        totalWinRound += board[2][0].value * bet;
                        hasWinLine = true;
                        winCells[2][0] = winCells[1][1] = winCells[0][2] = true;
                    }

                    // สรุปผลลัพธ์
                    if (hasWinLine) {
                        p.credit += totalWinRound; p.win_count++;
                        sysMsg = TextFormat("WIN! +$%d!", totalWinRound); msgColor = GREEN;
                    } else {
                        p.loss_count++;
                        sysMsg = "No win this time."; msgColor = LIGHTGRAY;
                    }
                }
            }
        } else if (gameState == 3) { 
            //อยู่ในหน้าวิธีเล่น ถ้ากด Back ให้กลับไปหน้า 0
            if (isClick && CheckCollisionPointRec(mousePos, btnBack)) gameState = 0;
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////
        //ส่วนของการวาดกราฟิก
        BeginDrawing();
        ClearBackground(Color{ 10, 30, 70, 255 }); // สีพื้นหลังโทนน้ำเงินเข้ม
        
        if (gameState == 3) {
            // วาดรูปสอนเล่น
            if (htpImg.id != 0) DrawTexture(htpImg, 0, 0, WHITE);
            // วาดปุ่ม Back สไตล์มาตรฐาน
            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON); 
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);
        } else {
            // ส่วนหัว (Title และ Credits)
            DrawText("SCAMMER SLOT", centerX - MeasureText("SCAMMER SLOT", 40)/2, 20, 40, GOLD);
            DrawText(TextFormat("Credits: $%.2f", p.credit), 1050, 30, 25, GOLD);
            
            // ตำแหน่งตู้สล็อตให้อยู่กึ่งกลาง
            int bSize = 120, spc = 10; //ขนาดของช่องสล็อตแต่ละช่อง และ ระยะห่างระหว่างช่อง
            int gW = (bSize * 3) + (spc * 2); //ความกว้างรวมของตารางสล็อต (ขนาดช่อง x 3 ช่อง) + (ช่องว่างที่คั่นกลาง 2 จุด)
            int stX = (screenWidth - gW) / 2; // จุดกึ่งกลางจอของตู้
            int stY = 140; // ระยะห่างจากขอบจอ

            // วาดกรอบตู้สล็อต
            DrawRectangle(stX - 20, stY - 20, gW + 40, (bSize * 3) + (spc * 2) + 40, Color{ 5, 15, 40, 255 }); 
            DrawRectangleLinesEx({(float)stX - 20, (float)stY - 20, (float)gW + 40, (float)(bSize * 3) + (spc * 2) + 40}, 6, GOLD); 

            // วนลูปวาดช่องสล็อต 3x3
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int cellX = stX + (j * (bSize + spc));
                    int cellY = stY + (i * (bSize + spc));
                    // ถ้าช่องนั้นชนะ ให้เปลี่ยนเป็นสีเขียว (LIME) ถ้าไม่ให้เป็นสีขาว (RAYWHITE)
                    DrawRectangle(cellX, cellY, bSize, bSize, winCells[i][j] ? LIME : RAYWHITE);
                    // วาดตัวหนังสือสัญลักษณ์
                    DrawText(board[i][j].name.c_str(), cellX + (bSize - MeasureText(board[i][j].name.c_str(), 35))/2, cellY + 40, 35, winCells[i][j] ? BLACK : board[i][j].color);
                }
            }

            // แสดงข้อความแจ้งเตือนผลลัพธ์
            DrawText(sysMsg.c_str(), centerX - MeasureText(sysMsg.c_str(), 30)/2, 550, 30, msgColor);

            // ส่วนควบคุม
            if (!isSpinning) {
                DrawText("BET AMOUNT:", boxBet.x - 140, boxBet.y + 12, 20, RAYWHITE);
                DrawRectangleRec(boxBet, betBoxActive ? RAYWHITE : LIGHTGRAY);
                // ถ้าเลือกช่อง Bet ให้วาดเส้นขอบสีทอง (Active State)
                DrawRectangleLinesEx(boxBet, 2, betBoxActive ? GOLD : BLACK);
                DrawText(betInput.c_str(), boxBet.x + 15, boxBet.y + 8, 30, BLACK);
                
                // ปุ่มวิธีเล่น
                DrawRectangleRec(btnHTP, CheckCollisionPointRec(mousePos, btnHTP) ? GRAY : DARKGRAY);
                DrawText("HOW TO PLAY", btnHTP.x + 15, btnHTP.y + 10, 18, RAYWHITE);
            }

            // ปุ่ม SPIN (เปลี่ยนเป็นสีเทาและเปลี่ยนคำถ้ากำลังหมุน)
            DrawRectangleRec(btnSpin, isSpinning ? DARKGRAY : RED);
            DrawRectangleLinesEx(btnSpin, 4, GOLD);
            DrawText(isSpinning ? "SPINNING..." : "SPIN", btnSpin.x + (btnSpin.width - MeasureText(isSpinning ? "SPINNING..." : "SPIN", 35))/2, btnSpin.y + 10, 35, GOLD);
            
            // ปุ่ม Back
            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON); 
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);

            // ตารางสรุปรางวัลด้านข้าง
            int ptX = 950, ptY = 200; 
            DrawText("PAYTABLE", ptX - 10, ptY - 40, 30, GOLD);
            DrawRectangle(ptX - 20, ptY - 10, 200, 340, Color{ 0, 0, 0, 150 }); 
            DrawRectangleLines(ptX - 20, ptY - 10, 200, 340, DARKGRAY); 
            for (int i = 0; i < (int)slotSymbols.size(); i++) {
                int lineY = ptY + (i * 40); 
                DrawText(slotSymbols[i].name.c_str(), ptX + 10, lineY + 10, 25, slotSymbols[i].color);
                DrawText(TextFormat("x %d", slotSymbols[i].value), ptX + 100, lineY + 10, 25, RAYWHITE);
            }
        }
        EndDrawing();
    }
    UnloadTexture(htpImg);
}

#endif