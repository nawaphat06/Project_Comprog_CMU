#ifndef BLACKJACK_UI_H
#define BLACKJACK_UI_H

#include "raylib.h"
#include "Player.h"
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>
#include <cstdlib> 

using namespace std;

//เก็บข้อมูลไพ่แต่ละใบ
struct Card {
    string rank; // หน้าไพ่
    string suit; // ดอกไพ่
    int value;   // แต้มของไพ่
};

// สร้างสำรับไพ่ 52 ใบ และทำการสับไพ่
static vector<Card> CreateDeck() {
    vector<Card> deck;
    string ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    int values[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11}; // กำหนดค่า A เริ่มต้นเป็น 11
    string suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
    
    // ลูปสร้างไพ่จนครบทุกดอกและทุกหน้า
    for (string s : suits) {
        for (int i = 0; i < 13; i++) deck.push_back({ranks[i], s, values[i]});
    }
    
    //สับไพ่
    int n = deck.size();
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(deck[i], deck[j]);
    }
    return deck;
}

// คำนวณแต้มรวมและจัดการค่าของ Ace (A)
static int CalculateScore(vector<Card> hand) {
    int score = 0, aces = 0;
    for (auto c : hand) {
        score += c.value;
        if (c.rank == "A") aces++; //นับจำนวนไพ่ Ace ในมือ
    }
    // ถ้าแต้มรวมเกิน 21 และมี Ace อยู่ ให้ลดแต้ม Ace จาก 11 เหลือ 1
    while (score > 21 && aces > 0) { 
        score -= 10; 
        aces--; 
    }
    return score;
}

//คำนวณแต้มเจ้ามือที่แสดงผลบนจอ (ปิดใบแรกไว้ขณะเล่น)
static int CalculateVisibleDealerScore(vector<Card> hand, bool isPlaying) {
    if (isPlaying && hand.size() >= 2) {
        vector<Card> visibleHand;
        //ดึงไพ่ใบที่ 2 เป็นต้นไปมาคำนวณ เพื่อแสดงแต้มเฉพาะใบที่เปิด
        for (size_t i = 1; i < hand.size(); i++) { 
            visibleHand.push_back(hand[i]); 
        }
        return CalculateScore(visibleHand);
    }
    return CalculateScore(hand); //ถ้าจบเกมแล้ว ให้โชว์แต้มเต็มทั้งหมด
}

void playBlackjackUI(Player &p) {
    //ตั้งค่าตำแหน่งหน้าจอ
    const int screenWidth = 1280;
    const int screenHeight = 720;
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;

    //โหลดรูปภาพสอนเล่น
    Texture2D htpBJ = LoadTexture("howtoplay/bj.png");

    vector<Card> deck = CreateDeck(); // เตรียมสำรับ
    vector<Card> playerHand, dealerHand; // มือของผู้เล่นและเจ้ามือ
    
    int bet = 0;
    string betInput = "50"; 
    bool betBoxActive = false; //สถานะกล่องใส่เงินเดิมพัน

    // gameState: 0=วางเดิมพัน, 1=กำลังตัดสินใจ (Hit/Stand), 2=สรุปผลชนะ/แพ้, 3=วิธีเล่น
    int gameState = 0; 
    string statusMsg = "Enter Bet and Deal!";
    Color msgColor = RAYWHITE;

    //กำหนดพิกัดปุ่มและ UI
    Rectangle btnBack      = { 30, 30, 100, 40 };
    Rectangle btnHTP       = { 30, 80, 150, 40 }; 
    Rectangle boxBet       = { centerX - 100, 530, 200, 45 }; 
    Rectangle btnDeal      = { centerX - 100, 610, 200, 55 };
    Rectangle btnHit       = { centerX - 220, 610, 200, 55 };
    Rectangle btnStand     = { centerX + 20, 610, 200, 55 };

    int frameDelay = 0; // กันบัคคลิกเบิ้ลนะน้อง

    while (!WindowShouldClose()) {
        frameDelay++;
        Vector2 mousePos = GetMousePosition();
        // เช็คการคลิกพร้อมตัวหน่วงเวลา
        bool isClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && (frameDelay > 10);

        //ส่วนของการประมวลผล
        if (gameState == 0) { // หน้าวางเดิมพัน
            if (isClick) {
                if (CheckCollisionPointRec(mousePos, btnBack)) { UnloadTexture(htpBJ); return; }
                if (CheckCollisionPointRec(mousePos, btnHTP)) gameState = 3;
                
                if (CheckCollisionPointRec(mousePos, boxBet)) betBoxActive = true;
                else betBoxActive = false;
                
                if (CheckCollisionPointRec(mousePos, btnDeal)) {
                    bet = atoi(betInput.c_str()); 
                    if (p.credit >= bet && bet > 0) {
                        p.credit -= bet; // หักเงินเดิมพัน
                        deck = CreateDeck(); // ล้างและสับไพ่ใหม่ทุกรอบ
                        playerHand.clear(); dealerHand.clear();
                        
                        //แจกไพ่เริ่มต้นฝ่ายละ 2 ใบ
                        playerHand.push_back(deck.back()); deck.pop_back();
                        dealerHand.push_back(deck.back()); deck.pop_back();
                        playerHand.push_back(deck.back()); deck.pop_back();
                        dealerHand.push_back(deck.back()); deck.pop_back();
                        
                        gameState = 1; //เข้าสู่สถานะเล่นเกม
                        betBoxActive = false; 
                        statusMsg = "HIT or STAND?";
                        msgColor = RAYWHITE;

                        //ถ้าผู้เล่นได้ 21 ทันที (Blackjack)
                        if (CalculateScore(playerHand) == 21) {
                            gameState = 2;
                            p.credit += bet * 2.5; // จ่ายโบนัส 1.5 เท่า (รวมทุนเป็น 2.5)
                            p.win_count++;
                            statusMsg = "BLACKJACK! YOU WIN!"; msgColor = GREEN;
                        }
                    } else {
                        statusMsg = (bet <= 0) ? "Bet must be > 0!" : "Not enough credits!";
                        msgColor = RED;
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

        } else if (gameState == 1) { // ช่วงเล่นเกม: ตัดสินใจว่าจะสู้ต่อหรือหยุด
            if (isClick) {
                if (CheckCollisionPointRec(mousePos, btnBack)) { p.loss_count++; UnloadTexture(htpBJ); return; }

                // [Action] จั่วไพ่ (HIT)
                if (CheckCollisionPointRec(mousePos, btnHit)) {
                    playerHand.push_back(deck.back()); deck.pop_back();
                    dealerHand.push_back(deck.back()); deck.pop_back();

                    // เช็คเงื่อนไขแต้มเกิน
                    if (CalculateScore(playerHand) > 21) {
                        gameState = 2; p.loss_count++;
                        statusMsg = "BUST! YOU LOSE."; msgColor = RED;
                    } else if (CalculateScore(dealerHand) > 21) {
                        gameState = 2; p.credit += bet * 2; p.win_count++;
                        statusMsg = "ENEMY BUSTS! YOU WIN!"; msgColor = GREEN;
                    }
                }
                // [Action] หยุดจั่ว (STAND) - วัดแต้มทันที
                else if (CheckCollisionPointRec(mousePos, btnStand)) {
                    gameState = 2;
                    int pS = CalculateScore(playerHand);
                    int dS = CalculateScore(dealerHand);
                    
                    // ตัดสินผลแพ้ชนะตามแต้มรวม
                    if (dS > 21 || pS > dS) { p.credit += bet * 2; p.win_count++; statusMsg = "YOU WIN!"; msgColor = GREEN; }
                    else if (pS < dS) { p.loss_count++; statusMsg = "ENEMY WINS!"; msgColor = RED; }
                    else { p.credit += bet; statusMsg = "PUSH (DRAW)"; msgColor = YELLOW; }
                }
            }
        } else if (gameState == 2) { // หน้าสรุปผล
            if (isClick) {
                if (CheckCollisionPointRec(mousePos, btnBack)) { UnloadTexture(htpBJ); return; }
                // คลิก Deal เพื่อกลับไปหน้าวางเดิมพันใหม่
                if (CheckCollisionPointRec(mousePos, btnDeal)) { gameState = 0; statusMsg = "Enter Bet and Deal!"; msgColor = RAYWHITE; }
            }
        } else if (gameState == 3) { // หน้าวิธีเล่น
            if (isClick && CheckCollisionPointRec(mousePos, btnBack)) gameState = 0;
        }

        //ส่วนของการวาดกราฟิก
        BeginDrawing();
        ClearBackground(Color{ 10, 60, 20, 255 }); // สีพื้นหลังเขียว

        if (gameState == 3) {
            if (htpBJ.id != 0) DrawTexture(htpBJ, 0, 0, WHITE);
            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON); 
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);
        } else {
            //แสดงชื่อเกมและยอดเงิน
            DrawText("BLACKJACK 21", centerX - MeasureText("BLACKJACK 21", 40)/2, 20, 40, GOLD);
            DrawText(TextFormat("Credits: $%.2f", p.credit), 1050, 30, 25, WHITE);

            //วาดไพ่ของเจ้ามือ (Enemy)
            if (gameState == 1 || gameState == 2) {
                DrawText(TextFormat("ENEMY'S HAND (%d)", CalculateVisibleDealerScore(dealerHand, (gameState == 1))), centerX - 100, 95, 20, LIGHTGRAY);
            }
            float dealerStartX = centerX - (dealerHand.size() * 110) / 2.0f;
            for (int i = 0; i < dealerHand.size(); i++) {
                Rectangle r = { dealerStartX + (i * 110), 120.0f, 100.0f, 140.0f };
                DrawRectangle(r.x + 5, r.y + 5, r.width, r.height, Color{0,0,0,80}); // วาดเงาใต้ไพ่
                
                // ขณะเล่น (State 1) ให้ "ปิดไพ่ใบแรก" ของเจ้ามือไว้
                if (gameState == 1 && i == 0) {
                    DrawRectangleRec(r, DARKGRAY); DrawText("?", r.x+35, r.y+40, 60, BLACK);
                } else {
                    DrawRectangleRec(r, RAYWHITE);
                    Color c = (dealerHand[i].suit == "Hearts" || dealerHand[i].suit == "Diamonds") ? RED : BLACK;
                    DrawText(dealerHand[i].rank.c_str(), r.x+8, r.y+8, 25, c);
                    DrawText(dealerHand[i].suit.c_str(), r.x+8, r.y+35, 12, c);
                }
            }

            //วาดไพ่ของผู้เล่น (Player)
            if (gameState == 1 || gameState == 2) {
                DrawText(TextFormat("YOUR HAND (%d)", CalculateScore(playerHand)), centerX - 80, 285, 20, LIGHTGRAY);
            }
            float playerStartX = centerX - (playerHand.size() * 110) / 2.0f;
            for (int i = 0; i < playerHand.size(); i++) {
                Rectangle r = { playerStartX + (i * 110), 310.0f, 100.0f, 140.0f };
                DrawRectangle(r.x + 5, r.y + 5, r.width, r.height, Color{0,0,0,80});
                DrawRectangleRec(r, RAYWHITE);
                Color c = (playerHand[i].suit == "Hearts" || playerHand[i].suit == "Diamonds") ? RED : BLACK;
                DrawText(playerHand[i].rank.c_str(), r.x+8, r.y+8, 25, c);
                DrawText(playerHand[i].suit.c_str(), r.x+8, r.y+35, 12, c);
            }

            //ส่วนแสดงข้อความแจ้งผลลัพธ์
            DrawText(statusMsg.c_str(), centerX - MeasureText(statusMsg.c_str(), 30)/2, 470, 30, msgColor);
            DrawRectangle(140, 510, 1000, 180, Color{0, 0, 0, 100}); // พื้นหลังแผงควบคุม

            //ปุ่มกดตามสถานะของเกม
            if (gameState == 0) {
                DrawText("BET AMOUNT:", boxBet.x - 140, boxBet.y + 12, 20, RAYWHITE);
                DrawRectangleRec(boxBet, betBoxActive ? RAYWHITE : LIGHTGRAY);
                DrawRectangleLinesEx(boxBet, 2, betBoxActive ? GOLD : BLACK); // ขอบสีทองเมื่อกดที่ช่องใส่เงิน
                DrawText(betInput.c_str(), boxBet.x + 15, boxBet.y + 8, 30, BLACK);
                DrawRectangleRec(btnDeal, RED); DrawText("DEAL", btnDeal.x+70, btnDeal.y+15, 25, GOLD);
                
                DrawRectangleRec(btnHTP, CheckCollisionPointRec(mousePos, btnHTP) ? GRAY : DARKGRAY);
                DrawText("HOW TO PLAY", btnHTP.x+15, btnHTP.y+10, 18, RAYWHITE);
            } else if (gameState == 1) {
                DrawRectangleRec(btnHit, SKYBLUE); DrawText("HIT", btnHit.x+80, btnHit.y+15, 25, BLACK);
                DrawRectangleRec(btnStand, ORANGE); DrawText("STAND", btnStand.x+65, btnStand.y+15, 25, BLACK);
            } else if (gameState == 2) {
                DrawRectangleRec(btnDeal, RED); DrawText("PLAY AGAIN", btnDeal.x+35, btnDeal.y+15, 25, GOLD);
            }

            // ปุ่ม Back
            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON); 
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);
        }
        EndDrawing();
    }
    UnloadTexture(htpBJ);
}
#endif