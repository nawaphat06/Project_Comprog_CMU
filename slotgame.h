#ifndef SLOTGAME_H
#define SLOTGAME_H
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include "Player.h"
using namespace std;

// สัญลักษณ์และคะแนน
class Symbol {
    public:
        string name;
        int value;
        int weight;
};

vector<Symbol> symbols = {
    {"[GOLD]", 500, 2},{"[RED]", 100, 6},  {"[BLUE]", 50, 8}, {"[ GREEN ]", 30, 10}, {"[ K ] ", 15, 14}, {"[ Q ] ", 10, 15}, {"[ J ]" , 8, 20},{"[ 10 ]", 5 , 25}
};

// ฟังก์ชันสุ่มสัญลักษณ์ตามน้ำหนัก
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
    system("cls");
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
    srand(time(0));
    
    int bet;
    cout << "\n--- Welcome to ASSTECS SLOT ---" << endl;
    cout << "Current Credits: " << p.credit << endl;
    cout << "Enter your bet per spin: ";
    cin >> bet;

    // 3. เปลี่ยนเงื่อนไขลูปให้ใช้เงินจาก p.credit
    while (p.credit >= bet) {
        cout << "\nBalance: " << p.credit << " | Bet: " << bet << endl;
        cout << "Press Enter to Spin (or type '0' to exit)...";
        
        cin.ignore(); // ล้างบัฟเฟอร์
        if(cin.get() == '0') break; // ให้ทางออกผู้เล่นกลับไปหน้าเมนูหลัก

        p.credit -= bet; // หักเงินจริง
        int multiplier = 1;
        int totalWin = 0;
        bool hasWin = true;

        while (hasWin) {
            hasWin = false;
            vector<vector<Symbol>> board(3, vector<Symbol>(3));
            
            for(int i=0; i<3; i++)
                for(int j=0; j<3; j++) board[i][j] = spinSymbol();

            displayBoard(board, multiplier);
            
            // ตรวจรางวัล
            for (int i = 0; i < 3; i++) {
                if (board[i][0].name == board[i][1].name && board[i][1].name == board[i][2].name) {
                    int win = (board[i][0].value * bet) * multiplier; // ปรับรางวัลตามยอด Bet
                    totalWin += win;
                    hasWin = true; 
                    cout << ">> WIN! " << board[i][0].name << " get " << win << "!" << endl;
                }
            }

            if (hasWin) {
                multiplier++;
                cout << "COMBO! Multiplier increases to x" << multiplier << endl;
                Sleep(1500); // ลดเวลาลงนิดนึงให้เกมลื่นขึ้น
            }
        }

        // 4. สรุปผลเข้า Profile ผู้เล่น
        if (totalWin > 0) {
            cout << "TOTAL WIN THIS ROUND: " << totalWin << endl;
            p.credit += totalWin; // เพิ่มเงินคืนเข้ากระเป๋าหลัก
            p.win_count++;
        } else {
            cout << "No win this time." << endl;
            p.loss_count++;
        }
        cout << "----------------------------" << endl;
    }

    if (p.credit < bet) {
        cout << "Game Over! You're out of balance." << endl;
    }
}

#endif