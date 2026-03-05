#ifndef SLOTGAME_H
#define SLOTGAME_H

#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

// แก้บัคของ Windows
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>

#include "Player.h"

using namespace std;

struct Symbol {
    string name;
    int value;
    int weight;
};

// ปรับความยาวชื่อสัญลักษณ์ให้เท่ากัน (9 ตัวอักษร) เพื่อให้ตารางไม่เบี้ยว
vector<Symbol> symbols = {
    {" [GOLD]  ", 500, 2}, {"  [RED]  ", 100, 6}, {" [BLUE]  ", 50, 8}, 
    {"[ GREEN ]", 30, 10}, {"  [ K ]  ", 15, 14}, {"  [ Q ]  ", 10, 15}, 
    {"  [ J ]  ", 8, 20},  {" [ 10 ]  ", 5 , 25}
};

Symbol spinSymbol() {
    int totalWeight = 0;
    for (const auto& s : symbols) totalWeight += s.weight;
    int rnd = rand() % totalWeight;
    for (const auto& s : symbols) {
        if (rnd < s.weight) return s;
        rnd -= s.weight;
    }
    return symbols.back();
}

void displayBoard(const vector<vector<Symbol>>& board, int multiplier) {
    cout << "\n\n";
    cout << "----------------------------" << endl;
    cout << "      ASSTECS SLOT GAMES    " << endl;
    cout << "----------------------------" << endl;
    for (int i = 0; i < 3; i++) {
        cout << "  ";
        for (int j = 0; j < 3; j++) {
            cout << board[i][j].name << " ";
        }
        cout << endl;
    }
    cout << "----------------------------" << endl;
    cout << " CURRENT MULTIPLIER: x" << multiplier << endl;
    cout << "----------------------------" << endl;
}

void playSlot(Player &p) { 
    SetConsoleOutputCP(CP_UTF8);

    double &balance = p.credit; 
    
    int bet;
    cout << "\n--- Welcome to SLOT ---" << endl;
    cout << "Credits: " << balance << endl;
    cout << "Enter your bet (Type 0 to Exit to Menu): "; // จุดทางออกที่ 1: ก่อนเริ่มเกม
    cin >> bet;

    if (bet == 0) return; // กด 0 เพื่อกลับเมนูหลักทันที

    // ระบบป้องกันคนกรอกตัวอักษรหรือกรอกเงินติดลบ
    if (cin.fail() || bet < 0 || bet > balance) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "[System] Invalid bet amount!\n";
        return;
    }

    cin.ignore(1000, '\n');

    while (balance >= bet) {
        cout << "\nBalance: " << balance << " | Bet: " << bet << endl;
        cout << "Press Enter to Spin (or type '0' to exit)..."; // จุดทางออกที่ 2: ก่อนปั่นสล็อต
        
        string input;
        getline(cin, input);
        if(input == "0") break; // ออกจากลูปเกมกลับไปเมนูหลัก

        balance -= bet;
        int multiplier = 1;
        int totalWin = 0;
        bool hasWin = true;

        while (hasWin) {
            hasWin = false;
            vector<vector<Symbol>> board(3, vector<Symbol>(3));
            
            for(int i=0; i<3; i++)
                for(int j=0; j<3; j++) board[i][j] = spinSymbol();

            displayBoard(board, multiplier);
            
            for (int i = 0; i < 3; i++) {
                if (board[i][0].name == board[i][1].name && board[i][1].name == board[i][2].name) {
                    int win = board[i][0].value * multiplier * bet; 
                    totalWin += win;
                    hasWin = true; 
                    cout << ">> WIN! " << board[i][0].name << " get " << win << "!" << endl;
                }
            }

            if (hasWin) {
                multiplier++;
                cout << "COMBO! Multiplier increases to x" << multiplier << endl;
                Sleep(2000);
            }
        }

        if (totalWin > 0) {
            cout << "TOTAL WIN THIS ROUND: " << totalWin << endl;
            balance += totalWin;
            p.win_count++; 
        } else {
            cout << "No win this time." << endl;
            p.loss_count++; 
        }
        cout << "----------------------------" << endl;

        // แสดงสถิติล่าสุดหลังปั่นเสร็จแต่ละครั้ง
        cout << "\n--- UPDATED STATS ---" << endl;
        p.showProfile();
        cout << "---------------------\n" << endl;
    }

    if (balance < bet) {
        cout << "[System] Game Over! You don't have enough balance for this bet." << endl;
        Sleep(2000); 
    }
}
#endif