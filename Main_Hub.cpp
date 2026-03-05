#include <iostream>
#include <string>
#include <windows.h> // จำเป็นต้องใช้สำหรับ SetConsoleOutputCP

#include "Player.h"
#include "bingo.h" 
#include "slotgame.h"
#include "blackjack.h"
#include "Hilo.h"

using namespace std;

int main(){
    SetConsoleOutputCP(CP_UTF8); //ทำให้ภาษาไทยไม่เพี้ยน
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
        
        // --- เช็คเงินหมด (Bankrupt / Game Over) ---
        if (p1.credit <= 0) {
            cout << "\n========================================" << endl;
            cout << "          BANKRUPT! GAME OVER           " << endl;
            cout << "========================================" << endl;
            cout << "คุณหมดตัวแล้ว " << p1.name << "!" << endl;
            cout << "Final Stats -> Wins: " << p1.win_count << " | Losses: " << p1.loss_count << endl;
            cout << "\nต้องการเริ่มเกมใหม่หรือไม่?" << endl;
            cout << "1. เล่นใหม่ (รีเซ็ตเงินเป็น 1000)" << endl;
            cout << "0. ยอมแพ้และออกจากเกม" << endl;
            cout << "Select option: ";
            
            int restart;
            cin >> restart;
            
            if (restart == 1) {
                p1.credit = 1000.0; 
                p1.win_count = 0;
                p1.loss_count = 0;
                cout << "\n[System] รีเซ็ตบัญชีเรียบร้อย ขอให้โชคดี!" << endl;
                cin.ignore(1000, '\n'); // เคลียร์บัฟเฟอร์กันข้ามบรรทัด
                continue; // วนกลับไปเริ่มแสดงเมนูใหม่
            } else {
                cout << "\nGoodbye! See you next time." << endl;
                break; // จบโปรแกรม
            }
        }
        // ------------------------------------------

        p1.showProfile();
        cout << "=== MAIN HUB ===" << endl;
        cout << "พิมพ์ตัวเลขเพื่อเข้าเล่นแต่ละเกม" << endl;
        cout << "1. Play Blackjack" << endl;
        cout << "2. Play Slot" << endl;
        cout << "3. Play Hi-Lo" << endl;
        cout << "4. Play Bingo" << endl;
        cout << "พิมพ์ 0 เพื่อออกจาก game" << endl;
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