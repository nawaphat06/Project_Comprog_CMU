#ifndef PLAYER_H
#define PLAYER_H

#include<string>

using namespace std;


class Player{
    public:
        string name;
        float credit;
        int win_count;
        int loss_count; 

        //Constructor 
        Player(string input_name, float c, int win, int loss);
        
};


Player::Player(string input_name, float c = 1000.0, int win = 0, int loss = 0){
    name = input_name;
    credit = c;
    win_count = win;
    loss_count = loss;
}

#endif