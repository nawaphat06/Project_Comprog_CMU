#include <iostream>
#include <string>

#include "Player.h"
#include "bingo.h" //ต้องดึงไฟล์เกมของเพื่อนเข้ามาด้วย
#include "slotgame.h"
#include "blackjack.h"
#include "Hilo.h"

using namespace std;

int main(){
    //รับชื่อ + ต้อนรับ
    string name;
    cout << "Welcome to Casino Simulator" << endl;
    cout << "Please enter your name: ";
    getline(cin, name);

    //set_starter_Stat
    Player p1(name, 1000.0, 0, 0); 

    //Main loop
    int choice;
    while(true){
        p1.showProfile();
        cout << "=== MAIN HUB ===" << endl;
        cout << "พิมพ์ตัวเลขเพื่อเข้าเล่นแต่ละเกม" << endl;
        cout << "1. Play Blackjack" << endl;
        cout << "2. Play Slot" << endl;
        cout << "3. Play Hi-Lo" << endl;
        cout << "4. Play Bingo" << endl;
        cout << "พิมพ์ 0 เพื่อออกจาก minigame" << endl;
        cout << "Select option: ";
        cin >> choice;

        if (choice == 0) {
            cout << "Goodbye! See you next time." << endl;
            break;
        }

        // ล้างบัฟเฟอร์ Enter ก่อนเข้าเกม
        cin.ignore();

        // ส่ง p1 ไปแบบ Reference ทั้งก้อน
        switch (choice) {
            case 1:
                playBlackjack(p1);
                break;
            case 2:
                playSlot(p1);
                break;
            case 3:
                playHilo(p1);
                break;
            case 4:
                playBingo(p1);
                break;    
            default:
                cout << "\n[System] Invalid option!" << endl;
        }
    }

    return 0;
}