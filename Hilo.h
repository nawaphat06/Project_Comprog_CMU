#ifndef HILO_H
#define HILO_H

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include "Player.h"

using namespace std;

inline int rollDice() {
    return rand() % 6 + 1;
}

void playHilo(Player &p) {
    double &money = p.credit; 
    
    int bet;
    int choice;
    int winStreak = 0;
    bool play = true; 

    while (play && money > 0) {
        cout << "\n================================";
        cout << "\n       WELCOME TO HI-LO        ";
        cout << "\n================================";
        cout << "\nCredits: " << money;
        cout << "\nEnter bet (Type 0 to Exit to Menu): ";
        cin >> bet;
        
        // จุดออกที่ 1: ขณะวางเดิมพัน
        if (bet == 0) return;

        if (cin.fail() || bet < 0 || bet > money) { 
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n[System] Invalid bet amount!";
            continue;
        }

        cout << "\n1. High / Low\n2. Triple\n3. Sum\n0. Exit to Menu";
        cout << "\nChoose option: ";
        cin >> choice;

        // จุดออกที่ 2: ขณะเลือกโหมดการเล่น
        if (choice == 0) return;

        int hl = 0;
        int guessTriple = 0;
        int guessSum = 0;
        bool back_to_bet = false;

        switch (choice) {
            case 1:
                cout << "\n1. Low (4-10)\n2. High (11-17)\n0. Back to Exit";
                cout << "\nChoose (1 or 2): ";
                cin >> hl;
                // จุดออกที่ 3: ขณะเลือกสูง/ต่ำ
                if (hl == 0) { back_to_bet = true; break; }
                break;
            case 2:
                cout << "\nGuess triple number (1-6, 0 to Exit): ";
                cin >> guessTriple;
                // จุดออกที่ 4: ขณะเลือกเลขตอง
                if (guessTriple == 0) { back_to_bet = true; break; }
                break;
            case 3:
                cout << "\nGuess sum (3-18, 0 to Exit): ";
                cin >> guessSum;
                // จุดออกที่ 5: ขณะเลือกผลรวม
                if (guessSum == 0) { back_to_bet = true; break; }
                break;
            default:
                cout << "\n[System] Invalid choice!";
                continue;
        }

        if (back_to_bet) return; // ออกจากฟังก์ชันทันที
    
        int luckyNumber;
        while (true) {
            cout << "\nChoose Lucky Number (1-6, 0 to Exit): ";
            cin >> luckyNumber;
            // จุดออกที่ 6: ขณะเลือกเลขนำโชค
            if (luckyNumber == 0) return;
            if (luckyNumber >= 1 && luckyNumber <= 6) break;
            else cout << "\n[System] Invalid! Enter 1-6 only.";
        }
        
        string dummy;
        cout << "\nPress Enter to roll the dice (or type '0' to forfeit bet and exit)...";
        cin.ignore(1000, '\n');
        getline(cin, dummy);
        // จุดออกที่ 7: ก่อนทอยเต๋า (ยอมทิ้งเงินเดิมพันเพื่อออก)
        if (dummy == "0") {
            money -= bet; // หักเงินเดิมพันที่ลงไปแล้ว
            p.loss_count++;
            return;
        }

        int d1 = rollDice();
        int d2 = rollDice();
        int d3 = rollDice();
        int sum = d1 + d2 + d3;

        cout << "\n--------------------------------";
        cout << "\nDice Result: " << d1 << " - " << d2 << " - " << d3;
        cout << " (Sum = " << sum << ")";
        cout << "\n--------------------------------\n";
      
        bool win = false;
        int reward = 0;

        if (choice == 1) {
            if (!(d1 == d2 && d2 == d3)) {
                if (hl == 1 && sum >= 4 && sum <= 10) { win = true; reward = bet * 2; }
                else if (hl == 2 && sum >= 11 && sum <= 17) { win = true; reward = bet * 2; }
            }
        }
        else if (choice == 2) {
            if (d1 == d2 && d2 == d3 && d1 == guessTriple) { win = true; reward = bet * 5; }
        }
        else if (choice == 3) {
            if (sum == guessSum) { win = true; reward = bet * 3; }
        }

        int luckyCount = 0;
        if (d1 == luckyNumber) luckyCount++;
        if (d2 == luckyNumber) luckyCount++;
        if (d3 == luckyNumber) luckyCount++;

        if (luckyCount > 0) cout << "Lucky Match! +x" << luckyCount << " Bonus!\n";

        int luckyBonus = luckyCount * bet;
       
        if (win) {
            cout << ">>> YOU WIN! <<<\n";
            money += (reward - bet);
            winStreak++;
            p.win_count++;
        } else {
            cout << ">>> YOU LOSE! <<<\n";
            money -= bet;
            winStreak = 0;
            p.loss_count++;
        }

        if (luckyCount > 0) money += luckyBonus;

        if (winStreak == 2) {
            cout << "Bonus: 2x Win Streak (+10%)\n";
            money += bet * 0.1;
        }
        else if (winStreak >= 3) {
            cout << "Bonus: " << winStreak << "x Win Streak (+20%)\n";
            money += bet * 0.2;
        }

        cout << "\n--- UPDATED STATS ---";
        p.showProfile();
        cout << "---------------------\n";

        cout << "Play again? (1: Yes / 0: Exit to Menu): ";
        cin >> play;
        // จุดออกที่ 8: หลังจบเกม
        if (play == 0) return;
    }

    if (money <= 0) {
        cout << "\n[System] Game Over: Out of money!";
        p.showProfile();
    }
}

#endif