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
        
        // จุดที่ 1: หน้าวางเดิมพัน (ยังไม่เริ่มเกม กด 0 ไม่เสียตัง)
        if (bet == 0) break;

        if (cin.fail() || bet < 0 || bet > money) { 
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n[System] Invalid bet amount!\n";
            continue;
        }

        cout << "\n1. High / Low\n2. Triple\n3. Sum\n0. Exit to Menu";
        cout << "\nChoose option: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear(); cin.ignore(10000, '\n');
            cout << "\n[System] Invalid input!\n";
            continue;
        }

        // จุดที่ 2: เริ่มเกมแล้ว (กด 0 คือยอมแพ้ หักเงินทันที)
        if (choice == 0) {
            cout << "\n[System] You exit the game! Bet is lost.\n";
            money -= bet; // หักเงิน
            p.loss_count++; // เพิ่มสถิติแพ้
            break;
        }

        int hl = 0;
        int guessTriple = 0;
        int guessSum = 0;
        bool forfeit = false; // forfeit = Exit

        switch (choice) {
            case 1:
                cout << "\n1. Low (4-10)\n2. High (11-17)\n0. Exit";
                cout << "\nChoose (1 or 2): ";
                cin >> hl;
                if (hl == 0) forfeit = true;
                else if (hl != 1 && hl != 2) { cout << "\nInvalid choice!\n"; continue; }
                break;
            case 2:
                cout << "\nGuess triple number (1-6, 0 to Exit): ";
                cin >> guessTriple;
                if (guessTriple == 0) forfeit = true;
                else if (guessTriple < 1 || guessTriple > 6) { cout << "\nInvalid choice!\n"; continue; }
                break;
            case 3:
                cout << "\nGuess sum (3-18, 0 to Exit): ";
                cin >> guessSum;
                if (guessSum == 0) forfeit = true;
                else if (guessSum < 3 || guessSum > 18) { cout << "\nInvalid choice!\n"; continue; }
                break;
            default:
                cout << "\n[System] Invalid choice!\n";
                continue;
        }

        // จุดที่ 3: เช็คว่ากดยอมแพ้จากตอนเลือกฝั่งหรือไม่
        if (forfeit) {
            cout << "\n[System] You exit the game! Bet is lost.\n";
            money -= bet;
            p.loss_count++;
            break;
        }
    
        int luckyNumber;
        while (true) {
            cout << "\nChoose Lucky Number (1-6, 0 to Exit): ";
            cin >> luckyNumber;
            
            if (cin.fail()) { 
                cin.clear(); cin.ignore(10000, '\n'); 
                continue; 
            }

            if (luckyNumber == 0) {
                forfeit = true;
                break;
            }
            if (luckyNumber >= 1 && luckyNumber <= 6) break;
            else cout << "\n[System] Invalid! Enter 1-6 only.\n";
        }

        // จุดที่ 4: เช็คว่ากดยอมแพ้จากตอนเลือกเลขนำโชคหรือไม่
        if (forfeit) {
            cout << "\n[System] You exit the game! Bet is lost.\n";
            money -= bet;
            p.loss_count++;
            break;
        }
        
        string dummy;
        cout << "\nPress Enter to roll the dice (or type '0' to Exit)...";
        cin.ignore(1000, '\n');
        getline(cin, dummy);

        // จุดที่ 5: หน้าสุดท้ายก่อนทอยเต๋า ถ้าเปลี่ยนใจก็หักเงินเช่นกัน
        if (dummy == "0") {
            cout << "\n[System] You exit the game! Bet is lost.\n";
            money -= bet;
            p.loss_count++;
            break;
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
        if (play == 0) break;
    }

    if (money <= 0) {
        cout << "\n[System] Game Over: Out of money!\n";
        p.showProfile();
    }
}

#endif