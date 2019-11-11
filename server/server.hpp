#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <array>
#include <random>
#include <chrono>
#include <set>
#include <unordered_map>
struct Player{
    int number;
    std::string name;
    std::vector<std::pair<int, std::string>> cards;
    std::string hand;
    void getHand();
    void dealHand(int cardCount);
    void drawCards(std::string discards);
    void handValue();
    int value = 0;
    std::array<int, 5> highCards = {0, 0, 0, 0, 0};
};
int players_max = 0;
int player_turn = 0;
std::vector<std::pair<int, std::string>> deck;
std::vector<Player> players;
std::vector<int> client_socket;
std::array<std::string, 9> handValues = {"High card", "Pair", "Two pairs", "Three of a kind", 
                                         "Straight", "Flush", "Full house", "Four of a kind", 
                                         "Straight flush"};

std::string cardValues = "23456789TJQKA";

void shuffleDeck(){
    deck.clear();
    std::array<std::string, 4> suit = {"s", "h", "d", "c"};
    for(int j = 0; j < 4; ++j){
        for(int i = 2; i <= 14; ++i){
            deck.emplace_back(std::make_pair(i, suit[j]));
        }
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(deck.begin(), deck.end(), std::default_random_engine(seed));
}
void Player::dealHand(int cardCount){
    for(int i = 0; i < cardCount; ++i){
        cards.emplace_back(deck[i]);
    }
    std::sort(cards.rbegin(), cards.rend());
    deck.erase(deck.begin(), deck.begin() + cardCount);
}
void Player::getHand(){
    hand = "";
    for(int i = 0; i < cards.size(); ++i){
        hand += cardValues[cards[i].first - 2] + cards[i].second + " ";
    }
}
void Player::drawCards(std::string str){
    std::set<int> setDiscards;
    std::vector<int> discards;
    for(int i = 0; i < str.size(); ++i){
        int d = int(str[i] - 48);
        if(d >= 1 && d <= 5){
            setDiscards.insert(d);
        }
    }
    for(int i : setDiscards){
        discards.emplace_back(i);
    }
    std::sort(discards.rbegin(), discards.rend());
    for(int i : discards){
        cards.erase(cards.begin() + (i - 1));
    }
    for(int i = 0; i < discards.size(); ++i){
        cards.emplace_back(deck[i]);
    }
    std::sort(cards.rbegin(), cards.rend());
    deck.erase(deck.begin(), deck.begin() + discards.size());
}
void Player::handValue(){
    std::unordered_map<int, int> valueCount;
    for(int i = 0; i < 5; ++i){
        valueCount[cards[i].first]++;
    }
    std::vector<std::pair<int, int>> val;
    for (const auto& value : valueCount){
        val.emplace_back(value.second, value.first);
    }
    sort(val.rbegin(), val.rend());
    for(int i = 0; i < val.size(); ++i){
        highCards[i] = val[i].second;
    }
    //straight flushes, flushes, straights and high cards
    if(val[0].first == 1){
        bool flush = true;
        for(int i = 1; i < 5; i++){
            if(cards[i].second != cards[0].second){
                flush = false;
                break;
            }
        }
        //flush
        if(flush){
            if(cards[0].first - cards[4].first == 4){
                value = 8; //straight flush
                return;
            }
            if(cards[0].first == 14 && cards[1].first == 5){
                highCards[0] = 5;
                value = 8; //straight flush
                return;
            }
            value = 5; //flush
            return;
        }
        else{
            if(cards[0].first - cards[4].first == 4){
                value = 4; //straight
                return;
            }
            if(cards[0].first == 14 && cards[1].first == 5){
                highCards[0] = 5;
                value = 4; //straight
                return;
            }
            value = 0; //high card
            return;
        }
    }
    //pairs and two pairs
    else if(val[0].first == 2){
        if(val[1].first == 2){
            value = 2; //two pairs
            return;
        }
        else{
            value = 1; //one pair
            return;
        }
    }
    //trips and full houses
    else if(val[0].first == 3){
        if(val[1].first == 2){
            value = 6; //full house
            return;
        }
        else{
            value = 3; //three of a kind
            return;
        }
    }
    //four of a kind
    else if(val[0].first == 4){
        value = 7; //four of a kind
        return;
    }
}

#endif