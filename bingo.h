#ifndef BINGO_H 
#define BINGO_H 
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
        void randbingo(int *); 
        template<class T>
        void show_table(T [][5]); 
        void showsetnum(int [],int); 
        void setscorefalse(bool *); 
        void check(int [][5],bool [][5],int [],int); 
        bool checkscore(bool *,int &,int); 
};

    void randnum(int [],int &); 

void playBingo(Player &p){ 
    double bet;
    cout << "\n--- Welcome to BINGO ---" << endl;
    cout << "Credits: " << p.credit << endl;
    cout << "Enter your bet (Type 0 to Exit to Menu): "; 
    cin >> bet;
    
    // จุดออกที่ 1: เปลี่ยนใจไม่เล่นตั้งแต่หน้าวางเงิน (ไม่เสียเงิน)
    if(bet == 0) return; 

    if(cin.fail() || bet < 0){
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "[System] Invalid bet amount!" << endl;
        return;
    }

    if(p.credit < bet) {
        cout << "Not enough credits!" << endl;
        return; 
    }
    p.credit -= bet; // หักเงินเดิมพัน! ตั้งแต่จุดนี้ไปถ้ากดออกคือเสียเงิน

    bingo player;
    bingo enemy;
    int setnum[100];
    int count = 0;
    srand(time(0));
    
    enemy.setscorefalse(enemy.dataS);
    enemy.randbingo(enemy.dataP);
    cout << "enemy table" << endl;
    enemy.show_table(enemy.player_peper);

    player.setscorefalse(player.dataS);
    player.randbingo(player.dataP);
    cout << "you table" << endl;
    player.show_table(player.player_peper);
    
    string ans;
    cout << "do you want to pick another table?  [1]yes [2]no [0]Forfeit to Menu" << endl; 
    cin >> ans;
    
    // จุดออกที่ 2: กดออกตอนเลือกกระดาษ (เสียเงิน เพราะเห็นกระดาษแล้ว)
    if (ans == "0") {
        cout << "\n[System] You forfeited the game! Bet is lost." << endl;
        p.loss_count++;
        return;
    }

    do{
        if(ans == "yes" || ans == "1"){
            player.randbingo(player.dataP);
            cout << "you table" << endl;
            player.show_table(player.player_peper);
            cout << "do you want to pick another table?  [1]yes [2]no [0]Forfeit to Menu" << endl;
            cin >> ans;
            
            if (ans == "0") {
                cout << "\n[System] You forfeited the game! Bet is lost." << endl;
                p.loss_count++;
                return;
            }
        }else{
            if(ans != "no" && ans != "2"){
                cout << "please input (yes/no) or (1/2) or 0 to Forfeit" << endl;
                cin >> ans;
                
                if (ans == "0") {
                    cout << "\n[System] You forfeited the game! Bet is lost." << endl;
                    p.loss_count++;
                    return;
                }
            }
        }
    }while(ans != "no" && ans != "2");

    player.type_win = rand()%3;
    cout << "\nwin type is "; 
    if(player.type_win == 0){
        cout << "row win" << endl;
    }else if(player.type_win == 1){
        cout << "cross win" << endl;
    }else{
        cout << "colum win" << endl;
    }

    cout << "Press Enter to start drawing numbers! (or type '0' to Forfeit)" << endl;
    cin.ignore(1000, '\n'); 

    while(!player.checkscore(player.dataS,player.wincount,player.type_win) && !enemy.checkscore(enemy.dataS,enemy.wincount,player.type_win)){
        getline(cin, ans); 
        
        // จุดออกที่ 3: ระหว่างกำลังสุ่มเลข (เสียเงิน)
        if(ans == "0"){
            cout << "\n[System] You forfeited the game! Bet is lost." << endl;
            p.loss_count++; 
            return;
        }
        
        randnum(setnum,count);
        player.check(player.player_peper,player.score,setnum,count);
        enemy.check(enemy.player_peper,enemy.score,setnum,count);
        
        cout << "enemy table" << endl;
        enemy.show_table(enemy.score);
        cout << "you table" << endl;
        player.show_table(player.score);
        cout << "Numbers drawn so far: ";
        player.showsetnum(setnum,count);
        cout << "Press Enter to draw next number...(0 to Forfeit)" << endl;
    }

    cout << "\n==========================" << endl;
    if(player.wincount > enemy.wincount){
        cout << ">>> YOU WIN! <<<" << endl;
        cout << "player score : " << player.wincount << endl;
        
        double prize = bet * 2; 
        p.credit += prize;
        p.win_count++;
        cout << "You received " << prize << " credits!" << endl;
    }else if(enemy.wincount > player.wincount){
        cout << ">>> YOU LOSE! <<<" << endl;
        cout << "enemy table" << endl;
        enemy.show_table(enemy.score);
        cout << "enemy score : " << enemy.wincount << endl;
        
        p.loss_count++;
    } else {
        cout << ">>> TIE! <<<" << endl;
        cout << "player score: " << player.wincount << " | enemy score: " << enemy.wincount << endl;
        p.credit += bet; 
    }

    cout << "\n--- UPDATED PROFILE ---" << endl;
    p.showProfile();
    cout << "-----------------------" << endl;
    
    cout << "Press Enter to return to Main Menu...";
    getline(cin, ans);
}

// ---------------------------------------------------------
// ด้านล่างนี้คือฟังก์ชันในคลาสของเพื่อนคุณ ไม่มีการแตะต้องใดๆ
// ---------------------------------------------------------

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