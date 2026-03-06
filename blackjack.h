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

struct Card {
    string rank;
    string suit;
    int value;
};

static vector<Card> CreateDeck() {
    vector<Card> deck;
    string ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    int values[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11};
    string suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
    for (string s : suits) {
        for (int i = 0; i < 13; i++) deck.push_back({ranks[i], s, values[i]});
    }
    int n = deck.size();
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
    return deck;
}

static int CalculateScore(vector<Card> hand) {
    int score = 0, aces = 0;
    for (auto c : hand) {
        score += c.value;
        if (c.rank == "A") aces++;
    }
    while (score > 21 && aces > 0) { score -= 10; aces--; }
    return score;
}

static int CalculateVisibleDealerScore(vector<Card> hand, bool isPlaying) {
    if (isPlaying && hand.size() >= 2) {
        vector<Card> visibleHand;
        for (size_t i = 1; i < hand.size(); i++) { 
            visibleHand.push_back(hand[i]); 
        }
        return CalculateScore(visibleHand);
    }
    return CalculateScore(hand); 
}

void playBlackjackUI(Player &p) {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;

    // --- โหลดรูปภาพ How to play สำหรับ Blackjack ---
    Texture2D htpBJ = LoadTexture("howtoplay/bj.png");

    vector<Card> deck = CreateDeck();
    vector<Card> playerHand, dealerHand;
    
    int bet = 0;
    string betInput = "50"; 
    bool betBoxActive = false;

    // gameState: 0 = Menu/Bet, 1 = Playing, 2 = Game Over, 3 = How To Play
    int bjState = 0; 
    string statusMsg = "Enter Bet and Deal!";
    Color msgColor = RAYWHITE;

    // พิกัดปุ่มและ UI
    Rectangle boxBet      = { centerX - 100, 530, 200, 45 }; 
    Rectangle btnDeal     = { centerX - 100, 610, 200, 55 };
    Rectangle btnHit      = { centerX - 220, 610, 200, 55 };
    Rectangle btnStand    = { centerX + 20, 610, 200, 55 };
    Rectangle btnBack     = { 30, 30, 100, 40 };
    Rectangle btnHTP      = { 30, 80, 150, 40 }; // ปุ่มวิธีเล่น

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        bool isClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (bjState == 0) { // หน้า Bet
            if (isClick) {
                if (CheckCollisionPointRec(mousePos, btnBack)) {
                    UnloadTexture(htpBJ);
                    return;
                }
                if (CheckCollisionPointRec(mousePos, btnHTP)) bjState = 3;
                
                if (CheckCollisionPointRec(mousePos, boxBet)) betBoxActive = true;
                else betBoxActive = false;
                
                if (CheckCollisionPointRec(mousePos, btnDeal)) {
                    bet = atoi(betInput.c_str()); 

                    if (bet <= 0) {
                        statusMsg = "Bet must be greater than 0!";
                        msgColor = RED;
                    } else if (p.credit >= bet) {
                        p.credit -= bet; 
                        deck = CreateDeck();
                        playerHand.clear(); dealerHand.clear();
                        
                        playerHand.push_back(deck.back()); deck.pop_back();
                        dealerHand.push_back(deck.back()); deck.pop_back();
                        playerHand.push_back(deck.back()); deck.pop_back();
                        dealerHand.push_back(deck.back()); deck.pop_back();
                        
                        bjState = 1; // เปลี่ยนเป็น Playing
                        betBoxActive = false; 
                        statusMsg = "HIT or STAND?";
                        msgColor = RAYWHITE;

                        if (CalculateScore(playerHand) == 21) {
                            bjState = 2; // Game Over
                            p.credit += bet * 2.5; p.win_count++;
                            statusMsg = "BLACKJACK! YOU WIN!"; msgColor = GREEN;
                        }
                    } else {
                        statusMsg = "Not enough credits!";
                        msgColor = RED;
                    }
                }
            }

            if (betBoxActive) {
                int key = GetCharPressed();
                while (key > 0) {
                    if ((key >= '0') && (key <= '9') && betInput.length() < 7) {
                        betInput += (char)key;
                    }
                    key = GetCharPressed(); 
                }
                if (IsKeyPressed(KEY_BACKSPACE) && betInput.length() > 0) betInput.pop_back();
            }

        } else if (bjState == 1) { // หน้าเล่นเกม
            if (isClick) {
                if (CheckCollisionPointRec(mousePos, btnBack)) {
                    p.loss_count++; 
                    UnloadTexture(htpBJ);
                    return; 
                }

                if (CheckCollisionPointRec(mousePos, btnHit)) {
                    playerHand.push_back(deck.back()); deck.pop_back();
                    dealerHand.push_back(deck.back()); deck.pop_back();

                    int pS = CalculateScore(playerHand);
                    int dS = CalculateScore(dealerHand);

                    if (pS > 21 && dS <= 21) {
                        bjState = 2; p.loss_count++;
                        statusMsg = "BUST! YOU LOSE."; msgColor = RED;
                    } else if (dS > 21 && pS <= 21) {
                        bjState = 2; p.credit += bet * 2; p.win_count++;
                        statusMsg = "ENEMY BUSTS! YOU WIN!"; msgColor = GREEN;
                    } else if (pS > 21 && dS > 21) {
                        bjState = 2; p.credit += bet;
                        statusMsg = "BOTH BUST! PUSH."; msgColor = YELLOW;
                    }
                }
                else if (CheckCollisionPointRec(mousePos, btnStand)) {
                    bjState = 2;
                    int pS = CalculateScore(playerHand);
                    int dS = CalculateScore(dealerHand);
                    
                    if (dS > 21) { p.credit += bet * 2; p.win_count++; statusMsg = "ENEMY BUSTS! YOU WIN!"; msgColor = GREEN; }
                    else if (pS > dS) { p.credit += bet * 2; p.win_count++; statusMsg = "YOU WIN!"; msgColor = GREEN; }
                    else if (pS < dS) { p.loss_count++; statusMsg = "ENEMY WINS!"; msgColor = RED; }
                    else { p.credit += bet; statusMsg = "PUSH (DRAW)"; msgColor = YELLOW; }
                }
            }
        } else if (bjState == 2) { // Game Over
            if (isClick) {
                if (CheckCollisionPointRec(mousePos, btnBack)) {
                    UnloadTexture(htpBJ);
                    return; 
                }
                if (CheckCollisionPointRec(mousePos, btnDeal)) {
                    bjState = 0; msgColor = RAYWHITE; statusMsg = "Enter Bet and Deal!";
                }
            }
        } else if (bjState == 3) { // How to play
            if (isClick && CheckCollisionPointRec(mousePos, btnBack)) bjState = 0;
        }

        // --- เริ่มวาดกราฟิก ---
        BeginDrawing();
        ClearBackground(Color{ 10, 60, 20, 255 });

        if (bjState == 3) { // แสดงรูปวิธีเล่น
            if (htpBJ.id != 0) DrawTexture(htpBJ, 0, 0, WHITE);
            else DrawText("IMAGE NOT FOUND: howtoplay/bj.png", centerX - 250, centerY, 20, RED);

            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON); 
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);
        } else {
            DrawText("BLACKJACK 21 (SYNC DRAW)", (screenWidth - MeasureText("BLACKJACK 21 (SYNC DRAW)", 40)) / 2, 20, 40, GOLD);
            DrawText(TextFormat("Credits: $%.2f", p.credit), (screenWidth - MeasureText(TextFormat("Credits: $%.2f", p.credit), 25)) / 2, 60, 25, WHITE);

            // --- ไพ่ศัตรู ---
            if (bjState == 1 || bjState == 2) {
                int dScore = CalculateVisibleDealerScore(dealerHand, (bjState == 1));
                DrawText(TextFormat("ENEMY'S HAND (%d)", dScore), centerX - 100, 95, 20, LIGHTGRAY);
            }
            float dealerStartX = centerX - (dealerHand.size() * 110) / 2.0f;
            for (int i = 0; i < dealerHand.size(); i++) {
                Rectangle r = { dealerStartX + (i * 110), 120.0f, 100.0f, 140.0f };
                DrawRectangle(r.x + 6, r.y + 6, r.width, r.height, Color{0,0,0,100});
                if (bjState == 1 && i == 0) {
                    DrawRectangleRec(r, DARKGRAY); DrawText("?", r.x + 35, r.y + 40, 60, BLACK);
                } else if (!dealerHand.empty()) {
                    DrawRectangleRec(r, RAYWHITE);
                    Color c = (dealerHand[i].suit == "Hearts" || dealerHand[i].suit == "Diamonds") ? RED : BLACK;
                    DrawText(dealerHand[i].rank.c_str(), r.x + 8, r.y + 8, 25, c);
                    DrawText(dealerHand[i].suit.c_str(), r.x + 8, r.y + 35, 12, c);
                }
            }

            // --- ไพ่ผู้เล่น ---
            if (bjState == 1 || bjState == 2) {
                DrawText(TextFormat("YOUR HAND (%d)", CalculateScore(playerHand)), centerX - 80, 285, 20, LIGHTGRAY);
            }
            float playerStartX = centerX - (playerHand.size() * 110) / 2.0f;
            for (int i = 0; i < playerHand.size(); i++) {
                Rectangle r = { playerStartX + (i * 110), 310.0f, 100.0f, 140.0f };
                DrawRectangle(r.x + 6, r.y + 6, r.width, r.height, Color{0,0,0,100});
                DrawRectangleRec(r, RAYWHITE);
                Color c = (playerHand[i].suit == "Hearts" || playerHand[i].suit == "Diamonds") ? RED : BLACK;
                DrawText(playerHand[i].rank.c_str(), r.x + 8, r.y + 8, 25, c);
                DrawText(playerHand[i].suit.c_str(), r.x + 8, r.y + 35, 12, c);
            }

            DrawText(statusMsg.c_str(), (screenWidth - MeasureText(statusMsg.c_str(), 30)) / 2, 470, 30, msgColor);
            DrawRectangle(140, 510, 1000, 180, Color{0, 0, 0, 120});

            if (bjState == 0) {
                DrawText("BET AMOUNT:", boxBet.x - 140, boxBet.y + 12, 20, RAYWHITE);
                DrawRectangleRec(boxBet, betBoxActive ? RAYWHITE : LIGHTGRAY);
                DrawText(betInput.c_str(), boxBet.x + 15, boxBet.y + 8, 30, BLACK);
                DrawRectangleRec(btnDeal, RED); DrawText("DEAL", btnDeal.x + 70, btnDeal.y + 15, 25, GOLD);
                
                // ปุ่ม How To Play
                DrawRectangleRec(btnHTP, CheckCollisionPointRec(mousePos, btnHTP) ? GRAY : DARKGRAY);
                DrawText("HOW TO PLAY", btnHTP.x + 15, btnHTP.y + 10, 18, RAYWHITE);
            } else if (bjState == 1) {
                DrawRectangleRec(btnHit, SKYBLUE); DrawText("HIT", btnHit.x + 80, btnHit.y + 15, 25, BLACK);
                DrawRectangleRec(btnStand, ORANGE); DrawText("STAND", btnStand.x + 65, btnStand.y + 15, 25, BLACK);
            } else if (bjState == 2) {
                DrawRectangleRec(btnDeal, RED); DrawText("PLAY AGAIN", btnDeal.x + 35, btnDeal.y + 15, 25, GOLD);
            }

            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON); 
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);
        }
        EndDrawing();
    }
    UnloadTexture(htpBJ);
}
#endif