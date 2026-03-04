#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cmath>
using namespace std;

//BLUEPRINT DECK FOR PLAYER AND BOT

struct deck_name_value {
    string name;
    int value;
};

vector <deck_name_value> blueprint_deck = {
        // Diamonds
        {"ace of diamonds", 1}, {"two of diamonds", 2}, {"three of diamonds", 3}, {"four of diamonds", 4},
        {"five of diamonds", 5}, {"six of diamonds", 6}, {"seven of diamonds", 7}, {"eight of diamonds", 8},
        {"nine of diamonds", 9}, {"ten of diamonds", 10}, {"jack of diamonds", 10}, {"queen of diamonds", 10},
        {"king of diamonds", 10},

        // Clubs
        {"ace of clubs", 1}, {"two of clubs", 2}, {"three of clubs", 3}, {"four of clubs", 4},
        {"five of clubs", 5}, {"six of clubs", 6}, {"seven of clubs", 7}, {"eight of clubs", 8},
        {"nine of clubs", 9}, {"ten of clubs", 10}, {"jack of clubs", 10}, {"queen of clubs", 10},
        {"king of clubs", 10},

        // Hearts
        {"ace of hearts", 1}, {"two of hearts", 2}, {"three of hearts", 3}, {"four of hearts", 4},
        {"five of hearts", 5}, {"six of hearts", 6}, {"seven of hearts", 7}, {"eight of hearts", 8},
        {"nine of hearts", 9}, {"ten of hearts", 10}, {"jack of hearts", 10}, {"queen of hearts", 10},
        {"king of hearts", 10},

        // Spades
        {"ace of spades", 1}, {"two of spades", 2}, {"three of spades", 3}, {"four of spades", 4},
        {"five of spades", 5}, {"six of spades", 6}, {"seven of spades", 7}, {"eight of spades", 8},
        {"nine of spades", 9}, {"ten of spades", 10}, {"jack of spades", 10}, {"queen of spades", 10},
        {"king of spades", 10}
    };

// Array or Vector of Player and Bot



vector <deck_name_value> PlayerDeck = blueprint_deck;
vector <deck_name_value> PlayerHand;
vector <deck_name_value> BotDeck = blueprint_deck;
vector <deck_name_value> BotHand;



int bot_total_card = 51, player_total_card = 51, player_card_total_value = 0, bot_card_total_value = 0;

void drawdeck(string which_type){

    if (which_type == "Player"){
        if (player_total_card > 0){
            int rng = rand()%player_total_card;
            cout <<"You drew "<< PlayerDeck[rng].name << "!\n";
            player_card_total_value += PlayerDeck[rng].value;
            PlayerHand.push_back(PlayerDeck[rng]);
            PlayerDeck.erase(PlayerDeck.begin() + rng);
            player_total_card--;
            }
        }
    

    else if (which_type == "Bot"){
        if (bot_total_card > 0){
            int rng = rand()%bot_total_card;
            bot_card_total_value += BotDeck[rng].value;
            BotHand.push_back(BotDeck[rng]);
            BotDeck.erase(BotDeck.begin() + rng);
            bot_total_card--;
            }
        }
}


int main() {


    srand(time(0));

    while(true){
        string status, dummy;
        bool player_can_draw = true;


        cout << "Press enter to draw the first two cards!";
        getline(cin, dummy);
        cout << "\n";
        drawdeck("Player"); // Stage 1: player gets the first card.
        drawdeck("Player"); // Stage 1: player gets the second card.


        cout << "\nNow you have: | ";
            for (int i = 0; i < PlayerHand.size(); i++){
                cout << PlayerHand[i].name << " | ";
            }
            cout << "\n\n" << "You have total value of cards in your hand of " << player_card_total_value <<"\n\n";

        while (player_can_draw){
            cout << "Hit(Type \"a\") or Stand(Type \"b\")\nType here >>"; // Stage 2: Player making decision
            getline(cin, status);

            if (status == "a" || status == "A"){
                cout << "\n";
                drawdeck("Player"); // Stage 3.1: Player gets another card
            
            }

            else if (status == "b" || status == "B"){
                cout << "\nYou chose Stand.\n";// Stage 3.2: Player did not want to get another card end the turn
                player_can_draw = false;

            }
            else { // Invalid input program killing
                cout << "\nInput error as it doesn't meet any criteria. Quiting the program.";
                return 0;
            } 

            if (player_can_draw){ //Result of Player's cards in their hand
                cout << "\nNow you have: | ";
                for (int i = 0; i < PlayerHand.size(); i++){
                    cout << PlayerHand[i].name << " | ";
                }
                cout << "\n\n" << "You have total value of cards in your hand of " << player_card_total_value <<"\n\n";
            }
            if (player_card_total_value > 21){ //Stage 3.3: Oopsie I have drawn too much
                player_can_draw = false;
                cout << "Cards in your hand have exceeded 21!\n";
            }
            if (player_card_total_value == 21){ //Stage 3.4: Lucky.
                player_can_draw = false;
                cout << "You got Blackjack!\n";
            }
        }

        drawdeck("Bot"); // Stage 4: dealer gets the first card.
        drawdeck("Bot"); // Stage 4: dealer gets the second card.

        vector <bool> true_or_false_spinwheel;

        int bot_want_to_draw = 1;

        int bot_times_drawn = 0;

        while(bot_want_to_draw == 1){ // Stage 5: dealer's decision to draw or not as roulette with RNG
            if (bot_card_total_value < 20){
    
                for (int i = 0;i < 21 - bot_card_total_value; i++){
                    true_or_false_spinwheel.push_back(1);
                }
                for (int i = 0;i < 21 - (21 - bot_card_total_value); i++){
                    true_or_false_spinwheel.push_back(0);
                }
                
                int rng = rand()%true_or_false_spinwheel.size();
                bot_want_to_draw = true_or_false_spinwheel[rng];

                // Still highest drawn card is 10, dealer can get 21 perfectly there is no risk in this draw
                if (bot_want_to_draw == 1 || bot_card_total_value <= 11){drawdeck("Bot"); bot_times_drawn++;}
                true_or_false_spinwheel.clear();
                
            }
            else break;
        }

        cout << "\nDealer has drawn: | "; //Stage 6 Dealer reveal
            for (int i = 0; i < BotHand.size(); i++){
                cout << BotHand[i].name << " | ";
            }
            cout << "\n\n" << "Dealer has total value of cards in your hand of " << bot_card_total_value <<"\n";
            cout << "You have total value of cards in your hand of " << player_card_total_value <<"\n";
            cout << "\n" << "Dealer has additional drawn: " << bot_times_drawn << " Time(s)."<<"\n\n";
        //Stage 7 judgement

        if (bot_card_total_value > 21){bot_card_total_value = 0;} //If dealer's cards exceeded

        if(player_card_total_value > bot_card_total_value && player_card_total_value <= 21){
            cout << "You win!";
        }

        else if(player_card_total_value == bot_card_total_value){
            cout << "Tie!";
        }
        
        else if(player_card_total_value >= 21 && bot_card_total_value >= 21){
            cout << "Tie!";
        }

        else{cout << "You lose!";}


        //New Round reset
        BotHand.clear();
        PlayerHand.clear();
        PlayerDeck = blueprint_deck;
        BotDeck = blueprint_deck;
        bot_total_card = 51, player_total_card = 51, player_card_total_value = 0, bot_card_total_value = 0;
        cout << "\n\n";
    }
    return 0;
}


