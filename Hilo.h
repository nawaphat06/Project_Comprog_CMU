#ifndef HILO_H
#define HILO_H

#include <iostream>
#include <cstdlib>
#include <ctime>
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
        cout << "\n========================\n";
        cout << "      HI-LO GAME        \n";
        cout << "========================\n";
        cout << "Money: " << money << endl;
        cout << "Enter bet: ";
        cin >> bet;
        
        if (cin.fail()) { 
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid bet!\n";
            continue;
        }
        
        if (bet <= 0 || bet > money) {
            cout << "Invalid bet!\n";
            continue;
        }

        cout << "1. High / Low\n";
        cout << "2. Triple\n";
        cout << "3. Sum\n";
        cout << "========================\n";
        cout << "Choose: ";
        cin >> choice;

        int hl = 0;
        int guessTriple = 0;
        int guessSum = 0;

        switch (choice) {
            case 1:
                cout << "1. Low (4-10)\n";
                cout << "2. High (11-17)\n";
                while (true) {
                    cout << "Choose (1 or 2): ";
                    cin >> hl;
                    if (hl == 1 || hl == 2) break;
                    else cout << "Invalid choice!\n";
                }
                break;
            case 2:
                cout << "Guess triple number (1-6): ";
                cin >> guessTriple;
                break;
            case 3:
                cout << "Guess sum (3-18): ";
                cin >> guessSum;
                break;
            default:
                cout << "Invalid choice!\n";
                continue;
        }
    
        int luckyNumber;
        while (true) {
            cout << "Choose Lucky Number (1-6): ";
            cin >> luckyNumber;
            if (luckyNumber >= 1 && luckyNumber <= 6) break;
            else cout << "Invalid! Enter 1-6 only.\n";
        }
        
        int d1 = rollDice();
        int d2 = rollDice();
        int d3 = rollDice();
        int sum = d1 + d2 + d3;

        cout << "\nDice: " << d1 << " " << d2 << " " << d3;
        cout << " (Sum = " << sum << ")\n";
      
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

        if (luckyCount > 0) {
            cout << "Matched Lucky Number " << luckyCount << " time(s)!\n";
        }

        int luckyBonus = luckyCount * bet;
       
        if (win) {
            cout << "You WIN!\n";
            money += (reward - bet);
            winStreak++;
            p.win_count++;
        } else {
            cout << "You LOSE!\n";
            money -= bet;
            winStreak = 0;
            p.loss_count++;
        }

        if (luckyCount > 0) {
            cout << "Lucky Bonus +" << luckyBonus << "!\n";
            money += luckyBonus;
        }

        if (winStreak == 2) {
            cout << "Win streak bonus +10%!\n";
            money += bet * 0.1;
        }
        else if (winStreak >= 3) {
            cout << "Win streak bonus +20%!\n";
            money += bet * 0.2;
        }

        cout << "\n--- UPDATED PROFILE ---" << endl;
        p.showProfile();
        cout << "-----------------------\n";

        cout << "Play again? (1=Yes / 0=No): ";
        cin >> play;
    }

    if (money <= 0) cout << "Game Over: Out of money!\n";
}

#endif