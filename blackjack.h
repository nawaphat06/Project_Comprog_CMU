#ifndef BLACKJACK_UI_H
#define BLACKJACK_UI_H

#include "raylib.h"
#include "Player.h"
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>

using namespace std;

// โครงสร้างไพ่
struct Card {
    string rank;
    string suit;
    int value;
};

// ฟังก์ชันสร้างและสับไพ่
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
        // สลับตำแหน่งไพ่
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
    return deck;
}

// ฟังก์ชันคำนวณแต้ม
static int CalculateScore(vector<Card> hand) {
    int score = 0, aces = 0;
    for (auto c : hand) {
        score += c.value;
        if (c.rank == "A") aces++;
    }
    while (score > 21 && aces > 0) { score -= 10; aces--; }
    return score;
}

void playBlackjackUI(Player &p) {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    vector<Card> deck = CreateDeck();
    vector<Card> playerHand, dealerHand;
    int bet = 50;
    bool playing = false; // สถานะขณะกำลังจั่วไพ่
    bool gameOver = false;
    string statusMsg = "Adjust Bet and Deal!";
    Color msgColor = RAYWHITE;

    Rectangle btnDeal  = { 540, 620, 200, 60 };
    Rectangle btnHit   = { 430, 620, 200, 60 };
    Rectangle btnStand = { 650, 620, 200, 60 };
    Rectangle btnMinus = { 460, 550, 50, 40 };
    Rectangle btnPlus  = { 770, 550, 50, 40 };
    Rectangle btnBack  = { 30, 30, 100, 40 };

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        bool isClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (!playing && !gameOver) {
            if (isClick) {
                if (CheckCollisionPointRec(mousePos, btnMinus) && bet > 10) bet -= 10;
                if (CheckCollisionPointRec(mousePos, btnPlus) && bet + 10 <= p.credit) bet += 10;
                if (CheckCollisionPointRec(mousePos, btnBack)) return;
                if (CheckCollisionPointRec(mousePos, btnDeal) && p.credit >= bet) {
                    p.credit -= bet;
                    deck = CreateDeck();
                    playerHand.clear(); dealerHand.clear();
                    playerHand.push_back(deck.back()); deck.pop_back();
                    dealerHand.push_back(deck.back()); deck.pop_back();
                    playerHand.push_back(deck.back()); deck.pop_back();
                    dealerHand.push_back(deck.back()); deck.pop_back();
                    playing = true;
                    statusMsg = "HIT or STAND?";
                }
            }
        } else if (playing) {
            if (isClick) {
                if (CheckCollisionPointRec(mousePos, btnHit)) {
                    playerHand.push_back(deck.back()); deck.pop_back();
                    if (CalculateScore(playerHand) > 21) {
                        playing = false; gameOver = true; p.loss_count++;
                        statusMsg = "BUST! YOU LOSE."; msgColor = RED;
                    }
                }
                if (CheckCollisionPointRec(mousePos, btnStand)) {
                    playing = false; gameOver = true;
                    while (CalculateScore(dealerHand) < 17) {
                        dealerHand.push_back(deck.back()); deck.pop_back();
                    }
                    int pS = CalculateScore(playerHand), dS = CalculateScore(dealerHand);
                    if (dS > 21 || pS > dS) { p.credit += bet * 2; p.win_count++; statusMsg = "YOU WIN!"; msgColor = GREEN; }
                    else if (pS < dS) { p.loss_count++; statusMsg = "DEALER WINS!"; msgColor = RED; }
                    else { p.credit += bet; statusMsg = "PUSH (DRAW)"; msgColor = YELLOW; }
                }
            }
        } else if (gameOver) {
            if (isClick && (CheckCollisionPointRec(mousePos, btnDeal) || CheckCollisionPointRec(mousePos, btnBack))) {
                if (CheckCollisionPointRec(mousePos, btnBack)) return;
                gameOver = false; msgColor = RAYWHITE; statusMsg = "Adjust Bet and Deal!";
            }
        }

        BeginDrawing();
        ClearBackground(Color{ 0, 80, 0, 255 });
        DrawText("BLACKJACK 21", (screenWidth - MeasureText("BLACKJACK 21", 40)) / 2, 30, 40, GOLD);
        DrawText(TextFormat("Credits: $%.2f", p.credit), (screenWidth - MeasureText(TextFormat("Credits: $%.2f", p.credit), 25)) / 2, 80, 25, WHITE);

        // วาดไพ่เจ้ามือ
        DrawText("DEALER'S HAND", 100, 130, 20, LIGHTGRAY);
        for (int i = 0; i < dealerHand.size(); i++) {
            DrawRectangle(100 + (i * 110), 160, 100, 140, WHITE);
            if (!gameOver && playing && i == 1) {
                DrawRectangle(100 + (i * 110), 160, 100, 140, DARKGRAY);
                DrawText("?", 140 + (i * 110), 210, 40, BLACK);
            } else {
                Color c = (dealerHand[i].suit == "Hearts" || dealerHand[i].suit == "Diamonds") ? RED : BLACK;
                DrawText(dealerHand[i].rank.c_str(), 110 + (i * 110), 170, 30, c);
                DrawText(dealerHand[i].suit.c_str(), 110 + (i * 110), 205, 15, c);
            }
        }

        // วาดไพ่ผู้เล่น
        DrawText(TextFormat("YOUR HAND (%d)", CalculateScore(playerHand)), 100, 350, 20, LIGHTGRAY);
        for (int i = 0; i < playerHand.size(); i++) {
            DrawRectangle(100 + (i * 110), 380, 100, 140, WHITE);
            Color c = (playerHand[i].suit == "Hearts" || playerHand[i].suit == "Diamonds") ? RED : BLACK;
            DrawText(playerHand[i].rank.c_str(), 110 + (i * 110), 390, 30, c);
            DrawText(playerHand[i].suit.c_str(), 110 + (i * 110), 425, 15, c);
        }

        DrawText(statusMsg.c_str(), (screenWidth - MeasureText(statusMsg.c_str(), 30)) / 2, 300, 30, msgColor);

        if (!playing && !gameOver) {
            DrawRectangleRec(btnMinus, GRAY); DrawText("-", 478, 555, 30, BLACK);
            DrawText(TextFormat("BET: %d", bet), 580, 560, 25, WHITE);
            DrawRectangleRec(btnPlus, GRAY); DrawText("+", 785, 555, 30, BLACK);
            DrawRectangleRec(btnDeal, GOLD); DrawText("DEAL", 605, 635, 25, BLACK);
        } else if (playing) {
            DrawRectangleRec(btnHit, SKYBLUE); DrawText("HIT", 505, 635, 25, BLACK);
            DrawRectangleRec(btnStand, ORANGE); DrawText("STAND", 710, 635, 25, BLACK);
        } else if (gameOver) {
            DrawRectangleRec(btnDeal, GOLD); DrawText("PLAY AGAIN", 565, 635, 25, BLACK);
        }
        
        DrawRectangleRec(btnBack, MAROON); DrawText("BACK", 50, 40, 20, WHITE);
        EndDrawing();
    }
}
#endif