#ifndef BINGO_H //เช็กว่าเคยอ่านหรือยัง
#define BINGO_H //ถ้ายัง ให้ประว่าอ่านแล้ว
#include<iostream>
#include<ctime>
#include<iomanip>
#include<string>
#include<vector>
#include "Player.h"

using namespace std;

class bingo
{
    public:
        int wincount = 0;
        int type_win;
        int player_peper[5][5];
        int *dataP = player_peper[0];
        bool score[5][5];
        bool *dataS = score[0];
        void randbingo(int *); //สุ่มกระดาษ
        template<class T>
        void show_table(T [][5]); //show table
        void showsetnum(int [],int); //show สุ่มเลขที่ออกไหม
        void setscorefalse(bool *); //ทำให้กระดาษเป็น false ทั้งหมด
        void check(int [][5],bool [][5],int [],int); //check เลขที่ออกตรงกับเลขในกระดาษไหม
        bool checkscore(bool *,int &,int); //check ชนะหรือยัง
};

    void randnum(int [],int &); //สุ่มเลขที่ออก

void playBingo(Player &p){ 
    double bet;
    cout << "\n--- Welcome to BINGO ---" << endl;
    cout << "Enter your bet: ";
    cin >> bet;
    
    // เช็คเงินว่าพอไหม
    if(p.credit < bet) {
        cout << "Not enough credits!" << endl;
        return; // เด้งกลับหน้าเมนู
    }
    p.credit -= bet; // หักเงินเดิมพัน

    bingo player;
    bingo enemy;
    int setnum[100];
    int count = 0;
    srand(time(0));
    
    //set enemy
    enemy.setscorefalse(enemy.dataS);
    enemy.randbingo(enemy.dataP);
    cout << "enemy table" << endl;
    enemy.show_table(enemy.player_peper);

    //set player
    player.setscorefalse(player.dataS);
    player.randbingo(player.dataP);
    cout << "you table" << endl;
    player.show_table(player.player_peper);
    
    //ผู้เล่นเลือกกระดาษ
    string ans;
    cout << "do you want to pick another table?  [1]yes [2]no [0]exit" << endl;
    cin >> ans;
    do{
        if(ans == "yes" || ans == "1"){
            player.randbingo(player.dataP);
            cout << "you table" << endl;
            player.show_table(player.player_peper);
            cout << "do you want to pick another table?  [1]yes [2]no [0]exit" << endl;
            cin >> ans;
        }else{
            if(ans != "no" && ans != "2"){
                cout << "please input (yes/no) or (1/2) or (0 to exit)" << endl;
                cin >> ans;
            }
            if(ans == "0"){
                p.credit += bet;
                return;
            }
        }
    }while(ans != "no" && ans != "2");

    //สุ่มว่าจะต้อง goal แบบไหนถึงชนะ
    player.type_win = rand()%3;
    cout << "win type is "; 
    if(player.type_win == 0){
        cout << "row win" << endl;
    }else if(player.type_win == 1){
        cout << "cross win" << endl;
    }else{
        cout << "colum win" << endl;
    }

    //เริ่มเกมจนกว่าจะชนะ
    cout << "Press Enter to start drawing numbers!" << endl;
    cin.ignore(); // ล้างบัฟเฟอร์

    while(!player.checkscore(player.dataS,player.wincount,player.type_win) && !enemy.checkscore(enemy.dataS,enemy.wincount,player.type_win)){
        getline(cin, ans); // รอรับการกด Enter
        
        randnum(setnum,count);
        player.check(player.player_peper,player.score,setnum,count);
        enemy.check(enemy.player_peper,enemy.score,setnum,count);
        
        cout << "enemy table" << endl;
        enemy.show_table(enemy.score);
        cout << "you table" << endl;
        player.show_table(player.score);
        player.showsetnum(setnum,count);
        cout << "Press Enter to draw next number...(0 to Exit)" << endl;
        if(ans == "0"){
            return;
        }
    }

    //บอกว่าชนะหรือแพ้ พร้อมคืนเงินเข้า Player p
    if(player.wincount > enemy.wincount){
        cout << "you win!" << endl;
        cout << "player score : " << player.wincount << endl;
        
        double prize = bet * 2; // ชนะได้เงิน 2 เท่า
        p.credit += prize;
        p.win_count++;
        cout << "You received " << prize << " credits!" << endl;
    }else{
        cout << "you lose!" << endl;
        cout << "enemy table" << endl;
        enemy.show_table(enemy.score);
        cout << "enemy score : " << enemy.wincount << endl;
        
        p.loss_count++;
    }
}

void bingo::randbingo(int *data){
    bool unique;
    for(int i=0;i<25;i++){
        do{
            *(data+i) = rand()%99+1;
            unique =true;
            for(int j=0;j<i;j++){
                if(*(data+i) == *(data+j)){
                    unique = false;
                }
            }
        }while(!unique);
    }
    *(data+12) = 0;
}

template<class T>
void bingo::show_table(T arr[][5]){
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++){
            cout << setw(2) << arr[i][j] << " ";
        }
        cout << endl;
    }
    cout << "------------------------------------------------" << endl;
}

void bingo::showsetnum(int arr[],int N){
    for(int i=0;i<N;i++){
        cout << arr[i] << " ";
    }
    cout << endl;
}

void bingo::check(int inarr[][5],bool score[][5],int setnum[],int N){
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++){
            if(inarr[i][j] == setnum[N-1]){
                score[i][j] = true;
            }
        }
    }
    score[2][2] = true;
}

void randnum(int arr[],int &count){
    bool unique;
    for(int i=0;i<count+1;i++){
        do{
            arr[count] = rand()%99+1;
            unique = true;
            for(int k=0;k<i;k++){
                if(arr[k] == arr[i]){
                    unique = false;
                }
            }
        }while(!unique);
    }
    count++;
}

void bingo::setscorefalse(bool *score){
    for(int i=0;i<25;i++){
        *(score+i) = false;
    }
}

bool bingo::checkscore(bool *data,int &wincount,int type_win){
    bool win = false;
    //row win
    if(type_win == 0){
        if(*(data) && *(data+1) && *(data+2) && *(data+3) && *(data+4)){
            win = true;
            wincount++;
        }
        if(*(data+5) && *(data+6) && *(data+7) && *(data+8) && *(data+9)){
            win = true;
            wincount++;
        }
        if(*(data+10) && *(data+11) && *(data+12) && *(data+13) && *(data+14)){
            win = true;
            wincount++;
        }
        if(*(data+15) && *(data+16) && *(data+17) && *(data+18) && *(data+19)){
            win = true;
            wincount++;
        }
        if(*(data+20) && *(data+21) && *(data+22) && *(data+23) && *(data+24)){
            win = true;
            wincount++;
        }
    }
    //cross win
    if(type_win == 1){
        if(*(data) && *(data+6) && *(data+12) && *(data+18) && *(data+24)){
            win = true;
            wincount++;
        }
        if(*(data+4) && *(data+8) && *(data+12) && *(data+16) && *(data+20)){
            win = true;
            wincount++;
        }
    }
    //colum win
    if(type_win == 2){
        if(*(data) && *(data+5) && *(data+10) && *(data+15) && *(data+20)){
            win = true;
            wincount++;
        }
        if(*(data+1) && *(data+6) && *(data+11) && *(data+16) && *(data+21)){
            win = true;
            wincount++;
        }
        if(*(data+2) && *(data+7) && *(data+12) && *(data+17) && *(data+22)){
            win = true;
            wincount++;
        }
        if(*(data+3) && *(data+8) && *(data+13) && *(data+18) && *(data+23)){
            win = true;
            wincount++;
        }
        if(*(data+4) && *(data+9) && *(data+14) && *(data+19) && *(data+24)){
            win = true;
            wincount++;
        }
    }
    return win;
}

#endif