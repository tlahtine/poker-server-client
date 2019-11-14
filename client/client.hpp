#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <bits/stdc++.h> 
int players = 0;
int players_in = 0;
int player_turn = 0;
int game_no;
std::unordered_map<std::string, std::string> suits = {
    {"s", "\u2664"},
    {"h", "\u2665"},
    {"d", "\u2666"},
    {"c", "\u2667"}};

std::string game_name = "Dpokr";
struct Player{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int port = 8888;
    std::string ipAddress = "127.0.0.1";
    std::string name;
    std::array<std::pair<std::string, std::string>, 5> cards;
    std::string handValue;
    std::vector<std::vector<std::string>> hand;
    void printHand();
    int number;
    bool added = false;
    bool host = false;
    bool cards_dealt = false;
    bool draw = false;
    bool shown = false;
};
void Player::printHand(){
    system("clear");
    hand.clear();
    for(int i = 0; i < 5; i++){
        std::vector<std::string> card = 
           {" ------------- ", 
            "|Vs           |",
            "|   -------   |",
            "|  |       |  |",
            "|  |       |  |",
            "|  |       |  |",
            "|  |   s   |  |",
            "|  |       |  |",
            "|  |       |  |",
            "|  |       |  |",
            "|   -------   |",
            "|             |",
            " ------------- "};

        card[1].replace(1, 1, cards[i].first);
        card[1].replace(2, 1, suits[cards[i].second]);
        card[6].replace(7, 1, suits[cards[i].second]);
        card[11].replace(7, 1, std::to_string(i + 1));
        hand.emplace_back(card);
    }
    for(int i = 0; i < 13; ++i){
        for(int j = 0; j < 5; ++j){
            std::cout << hand[j][i] << " ";
        }
    }
    std::cout << handValue << "\n";
}
#endif