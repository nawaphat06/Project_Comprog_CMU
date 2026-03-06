#ifndef PLAYER_H //เช็กว่าเคยอ่านหรือยัง
#define PLAYER_H //ถ้ายัง ให้จดไว้ว่าอ่านแล้ว

#include<string>


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

#endif //จบขอบเขต