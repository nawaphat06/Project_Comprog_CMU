#include<iostream>
#include<string>
#ifndef PLAYER_H //เช็กว่าเคยอ่านหรือยัง
#define PLAYER_H //ถ้ายัง ให้จดไว้ว่าอ่านแล้ว

using namespace std;

class Player{
    public:
        string name;
        double credit;
        int win_count;
        int loss_count;

        Player(string input_name, double c, int win, int loss);
        void showProfile();
};


Player::Player(string input_name, double c = 1000.0, int win = 0, int loss = 0){
    name = input_name;
    credit = c;
    win_count = win;
    loss_count = loss;
}

void Player::showProfile(){
    cout << "\n--- Player Profile ---" << endl;
    cout << "Name: " << name << endl;
    cout << "Credits: " << credit << " pts" << endl;
    cout << "Stats: Win " << win_count << " | Loss " << loss_count << endl;
    cout << "----------------------\n" << endl;
}

#endif //จบขอบเขต