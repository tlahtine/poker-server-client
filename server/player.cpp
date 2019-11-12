#include "player.hpp"
#include "game.hpp"
void Player::getHand(){
    hand = "";
    for(int i = 0; i < cards.size(); ++i){
        hand += cardValues[cards[i].first - 2] + cards[i].second + " ";
    }
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
