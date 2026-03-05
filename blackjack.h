#ifndef BLACKJACK_H
#define BLACKJACK_H

#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include "Player.h"

using namespace std;

struct deck_name_value {
    string name;
    int value;
};

void drawdeck_bj(string which_type, int &p_total, int &b_total, int &p_val, int &b_val, 
                 vector<deck_name_value> &P_Deck, vector<deck_name_value> &P_Hand, 
                 vector<deck_name_value> &B_Deck, vector<deck_name_value> &B_Hand) {

    if (which_type == "Player") {
        if (p_total > 0) {
            int rng = rand() % p_total;
            cout << "You drew " << P_Deck[rng].name << "!\n";
            p_val += P_Deck[rng].value;
            P_Hand.push_back(P_Deck[rng]);
            P_Deck.erase(P_Deck.begin() + rng);
            p_total--;
        }
    }
    else if (which_type == "Bot") {
        if (b_total > 0) {
            int rng = rand() % b_total;
            b_val += B_Deck[rng].value;
            B_Hand.push_back(B_Deck[rng]);
            B_Deck.erase(B_Deck.begin() + rng);
            b_total--;
        }
    }
}

void playBlackjack(Player &p) {
    double bet;
    cout << "\n--- Welcome to BLACKJACK ---" << endl;
    cout << "Your Credits: " << p.credit << endl;
    cout << "Enter your bet (0 to exit): ";
    cin >> bet;
    if (bet <= 0 || p.credit < bet) return;
    cin.ignore(1000, '\n');

    vector<deck_name_value> blueprint_deck = {
        {"ace of diamonds", 1}, {"two of diamonds", 2}, {"three of diamonds", 3}, {"four of diamonds", 4},
        {"five of diamonds", 5}, {"six of diamonds", 6}, {"seven of diamonds", 7}, {"eight of diamonds", 8},
        {"nine of diamonds", 9}, {"ten of diamonds", 10}, {"jack of diamonds", 10}, {"queen of diamonds", 10}, {"king of diamonds", 10},
        {"ace of clubs", 1}, {"two of clubs", 2}, {"three of clubs", 3}, {"four of clubs", 4},
        {"five of clubs", 5}, {"six of clubs", 6}, {"seven of clubs", 7}, {"eight of clubs", 8},
        {"nine of clubs", 9}, {"ten of clubs", 10}, {"jack of clubs", 10}, {"queen of clubs", 10}, {"king of clubs", 10},
        {"ace of hearts", 1}, {"two of hearts", 2}, {"three of hearts", 3}, {"four of hearts", 4},
        {"five of hearts", 5}, {"six of hearts", 6}, {"seven of hearts", 7}, {"eight of hearts", 8},
        {"nine of hearts", 9}, {"ten of hearts", 10}, {"jack of hearts", 10}, {"queen of hearts", 10}, {"king of hearts", 10},
        {"ace of spades", 1}, {"two of spades", 2}, {"three of spades", 3}, {"four of spades", 4},
        {"five of spades", 5}, {"six of spades", 6}, {"seven of spades", 7}, {"eight of spades", 8},
        {"nine of spades", 9}, {"ten of spades", 10}, {"jack of spades", 10}, {"queen of spades", 10}, {"king of spades", 10}
    };

    while (true) {
        if (p.credit < bet) {
            cout << "Out of credits!" << endl;
            break;
        }
        
        p.credit -= bet; 

        vector<deck_name_value> PlayerDeck = blueprint_deck;
        vector<deck_name_value> PlayerHand;
        vector<deck_name_value> BotDeck = blueprint_deck;
        vector<deck_name_value> BotHand;
        int bot_total_card = 51, player_total_card = 51, player_card_total_value = 0, bot_card_total_value = 0;

        string status, dummy;
        bool player_can_draw = true;
        bool exit_mid_game = false;

        cout << "\nPress enter to draw the first two cards!";
        getline(cin, dummy);
        
        drawdeck_bj("Player", player_total_card, bot_total_card, player_card_total_value, bot_card_total_value, PlayerDeck, PlayerHand, BotDeck, BotHand);
        drawdeck_bj("Player", player_total_card, bot_total_card, player_card_total_value, bot_card_total_value, PlayerDeck, PlayerHand, BotDeck, BotHand);

        while (player_can_draw) {
            cout << "\nNow you have: | ";
            for (int i = 0; i < PlayerHand.size(); i++) cout << PlayerHand[i].name << " | ";
            cout << "\nTotal value: " << player_card_total_value << "\n";

            if (player_card_total_value >= 21) {
                if(player_card_total_value == 21) cout << "Blackjack!\n";
                else cout << "Exceeded 21!\n";
                player_can_draw = false;
                break;
            }

            cout << "Hit(a), Stand(b) or Exit to Menu(0) >> "; 
            getline(cin, status);

            if (status == "0") {
                cout << "Exiting to menu... Refunded bet." << endl;
                p.credit += bet; 
                exit_mid_game = true;
                break; 
            }
            else if (status == "a" || status == "A") {
                drawdeck_bj("Player", player_total_card, bot_total_card, player_card_total_value, bot_card_total_value, PlayerDeck, PlayerHand, BotDeck, BotHand);
            } else {
                player_can_draw = false;
            }
        }

        if (exit_mid_game) break;

        // Bot Turn
        drawdeck_bj("Bot", player_total_card, bot_total_card, player_card_total_value, bot_card_total_value, PlayerDeck, PlayerHand, BotDeck, BotHand);
        drawdeck_bj("Bot", player_total_card, bot_total_card, player_card_total_value, bot_card_total_value, PlayerDeck, PlayerHand, BotDeck, BotHand);

        vector<bool> true_or_false_spinwheel;
        int bot_want_to_draw = 1;
        int bot_times_drawn = 0; // เพิ่มตัวแปรนับจำนวนการจั่วของ Bot กลับมาแล้ว

        while (bot_want_to_draw == 1) {
            if (bot_card_total_value < 20) {
                for (int i = 0; i < 21 - bot_card_total_value; i++) true_or_false_spinwheel.push_back(1);
                for (int i = 0; i < bot_card_total_value; i++) true_or_false_spinwheel.push_back(0);
                int rng = rand() % true_or_false_spinwheel.size();
                bot_want_to_draw = true_or_false_spinwheel[rng];
                
                if (bot_want_to_draw == 1 || bot_card_total_value <= 11) {
                    drawdeck_bj("Bot", player_total_card, bot_total_card, player_card_total_value, bot_card_total_value, PlayerDeck, PlayerHand, BotDeck, BotHand);
                    bot_times_drawn++; // นับจำนวนครั้งที่ Bot จั่วเพิ่ม
                }
                true_or_false_spinwheel.clear();
            } else break;
        }

        // --- เพิ่มการแสดงผลตอนจบตามโค้ดเก่าเป๊ะๆ ---
        cout << "\nDealer has drawn: | ";
        for (int i = 0; i < BotHand.size(); i++){
            cout << BotHand[i].name << " | ";
        }
        cout << "\n\nDealer has total value of cards in your hand of " << bot_card_total_value <<"\n";
        cout << "You have total value of cards in your hand of " << player_card_total_value <<"\n";
        cout << "\nDealer has additional drawn: " << bot_times_drawn << " Time(s).\n\n";
        // ------------------------------------------

        // Judgement
        int final_bot_val = (bot_card_total_value > 21) ? 0 : bot_card_total_value;

        if (player_card_total_value <= 21 && player_card_total_value > final_bot_val) {
            cout << "You win!" << endl;
            p.credit += (bet * 2);
            p.win_count++;
        } else if (player_card_total_value <= 21 && player_card_total_value == final_bot_val) {
            cout << "Tie!" << endl;
            p.credit += bet;
        } else {
            cout << "You lose!" << endl;
            p.loss_count++;
        }

        cout << "\n\n--- CURRENT STATS ---" << endl;
        p.showProfile(); 
        cout << "----------------------" << endl;

        cout << "\nPlay again? (1: Yes, 0: No): ";
        string again;
        getline(cin, again);
        if (again == "0") break;
    }
}

#endif