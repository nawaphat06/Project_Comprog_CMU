#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

int rollDice() {
    return rand() % 6 + 1;
}

int main() {
    srand(time(NULL));
    int money = 10000;
    int bet;
    int choice;
    int winStreak = 0;
    bool play = true; //เอาไว้กำหนดการเล่นใหม่
    
    //เอาไว้รับ bet
    while (play && money > 0) {
        cout << "========================\n";
        cout << "Money: " << money << endl;
        cout << "Enter bet: ";
        cin >> bet;
        cout << "========================\n";
        if (cin.fail()) { //ไว้เคลียร์หากใส่ betเยอะเกิน เช่น 33333333333333333333
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid bet!\n";
        continue;
        }
        
        if (bet <= 0 || bet > money) {
            cout << "Invalid bet!\n";
            continue;
        }
        //ไว้เเทงสูงต่ำ เลขตอง ผลรวม
        cout << "1. High / Low\n";
        cout << "2. Triple\n";
        cout << "3. Sum\n";
        cout << "========================\n";
        cout << "Choose: ";
        cin >> choice;
        cout << "========================\n";
        //ตัวเเปรไว้เก็บการเล่นทั้ง 3 เเบบ สูงต่ำ เลขตอง ผลรวม 
        int hl = 0;
        int guessTriple = 0;
        int guessSum = 0;

        // case การเดิมพัน
        switch (choice) {

            case 1:
                cout << "1. Low (4-10)\n";
                cout << "2. High (11-17)\n";
                cout << "========================\n";
                while (true) {
                cout << "Choose (1 or 2): ";
                cin >> hl;
                cout << "========================\n";
                if (hl == 1 || hl == 2) {
                    break;  
                }
                else {
                    cout << "Invalid choice! Please enter 1 or 2.\n";
                    cout << "========================\n";
                }
                }
                break;

            case 2:
                cout << "Guess triple number (1-6): ";
                cin >> guessTriple;
                cout << "\n========================\n";
                break;

            case 3:
                cout << "Guess sum (3-18): ";
                cin >> guessSum;
                cout << "\n========================\n";
                break;

            default:
                cout << "Invalid choice!\n";
                cout << "========================\n";
                continue;
        }
	
        // ระบบ Lucky number
        int luckyNumber;
        while (true) {
        cout << "Choose Lucky Number (1-6): ";
        cin >> luckyNumber;
        cout << "========================\n";
        cout << "| | | | | | | | | | | | \n";
        cout << "v v v v v v v v v v v v\n";

        if (luckyNumber >= 1 && luckyNumber <= 6) {
        break;
        }
        else {
        cout << "Invalid! Enter 1-6 only.\n";
        }
        }
        
        //ทอยลูกเต๋า
        int d1 = rollDice();
        int d2 = rollDice();
        int d3 = rollDice();
        int sum = d1 + d2 + d3;

        cout << "\nDice: " << d1 << " " << d2 << " " << d3;
        cout << " (Sum = " << sum << ")\n";
      
        bool win = false;
        int reward = 0;

        //ตรวจผลหลังทอย
        if (choice == 1) {
            if (!(d1 == d2 && d2 == d3)) {
                if (hl == 1 && sum >= 4 && sum <= 10) {
                    win = true;
                    reward = bet * 2;
                }
                else if (hl == 2 && sum >= 11 && sum <= 17) {
                    win = true;
                    reward = bet * 2;
                }
            }
        }
        else if (choice == 2) {
            if (d1 == d2 && d2 == d3 && d1 == guessTriple) {
                win = true;
                reward = bet * 5;
            }
        }
        else if (choice == 3) {
            if (sum == guessSum) {
                win = true;
                reward = bet * 3;
            }
        }

        //จำนวนที่Lucky 
        int luckyCount = 0;
        if (d1 == luckyNumber) luckyCount++;
        if (d2 == luckyNumber) luckyCount++;
        if (d3 == luckyNumber) luckyCount++;

        if (luckyCount > 0) {
        cout << "========================\n";
        cout << "You matched Lucky Number " << luckyCount << " time(s)!\n";
}

        int luckyBonus = luckyCount * bet; //บอกจำนวนเงินจาก lucky
       
        if (win) {
            cout << "========================\n";
            cout << "You WIN!\n";
            money += (reward - bet);
            winStreak++;
        } else {
            cout << "========================\n";
            cout << "You LOSE!\n";
            money -= bet;
            winStreak = 0;
        }

        if (luckyCount > 0) {
            cout << "========================\n";
            cout << "Lucky Bonus x" << luckyCount << "!\n";
            money += luckyBonus;
        }
        //ระบบชนะต่อเนื่อง
        if (winStreak == 2) {
            cout << "========================\n";
            cout << "Win streak bonus +10%!\n";
            money += bet * 0.1;
        }
        else if (winStreak >= 3) {
            cout << "========================\n";
            cout << "Win streak bonus +20%!\n";
            money += bet * 0.2;
        }
        cout << "========================\n";
        cout << "Current Money: " << money << endl;
        cout << "========================\n";
        cout << "Play again? (1=Yes / 0=No): ";
        cin >> play;
    }

    cout << "Game Over\n";
    return 0;
}