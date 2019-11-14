#include "game.hpp"
#include "server.hpp"
std::string compareHands(std::vector<int> val1, std::vector<int> val2, std::string player1, std::string player2){
    std::string bestHand = player1;
    for(int i = 0; i < val1.size(); ++i){
        if(val2[i] > val1[i]){
            bestHand = player2;
            break;
        }
        else if(val2[i] < val1[i]){
            break;
        }
    }
    return bestHand;
}
void Game::shuffleDeck(){
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
std::string Game::getWinner(){
    std::vector<int> values;
    std::unordered_map<std::string, std::vector<int>> handValues;
    for(auto a : players){
        values.emplace_back(a.value);
        for(int i = 0; i < 5; ++i){
            values.emplace_back(a.highCards[i]);
        }
        handValues[a.name] = values;
        values.clear();
    }
    std::string bestHand = players[0].name;
    for(int i = 1; i < players.size(); ++i){
        bestHand = compareHands(handValues[bestHand], handValues[players[i].name], bestHand, players[i].name);
    }
    return bestHand;
}
void Game::addPlayer(std::string name, int socket, int game_no){
    std::string msgOut;
    Player player;
    player.name = name;
    player.socket = socket;
    players.emplace_back(player);
    for(auto a : players){
        if(a.socket == socket){
            msgOut = "ADDP:" + name + ":" + std::to_string(socket) + 
                        ":" + std::to_string(players_max) + 
                        ":" + std::to_string(players.size()) + 
                        ":" + std::to_string(game_no);
        }
        else{
            msgOut = "JOIN:" + name;
        }
        std::cout << socket << "<< " << msgOut << "\n";
        send(a.socket, msgOut.c_str(), msgOut.size(), 0);
    }
}
void Game::dealHand(int socket){
    int player_no = 0;
    for(int i = 0; i < players.size(); ++i){
        if(players[i].socket == socket){
            player_no = i;
        }
    }
    for(int i = 0; i < cardCount; ++i){
        players[player_no].cards.emplace_back(deck[i]);
    }
    std::sort(players[player_no].cards.rbegin(), players[player_no].cards.rend());
    deck.erase(deck.begin(), deck.begin() + cardCount);
    players[player_no].handValue();
    players[player_no].getHand();
    std::string msgOut = "DEAL:" + std::to_string(players[player_turn].socket) + ":" + 
        players[player_no].hand + " " + handValues[players[player_no].value];
    std::cout << socket << "<< " << msgOut << "\n";
    send(socket, msgOut.c_str(), msgOut.size(), 0);
}
void Game::drawCards(std::string str, int socket){
    int player_no = 0;
    for(int i = 0; i < players.size(); ++i){
        if(players[i].socket == socket){
            player_no = i;
        }
    }
    std::vector<int> discards;
    if(str != "0"){
        std::set<int> setDiscards;
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
            players[player_no].cards.erase(players[player_no].cards.begin() + (i - 1));
        }
        for(int i = 0; i < discards.size(); ++i){
            players[player_no].cards.emplace_back(deck[i]);
        }
        std::sort(players[player_no].cards.rbegin(), players[player_no].cards.rend());
        deck.erase(deck.begin(), deck.begin() + discards.size());

        players[player_no].handValue();
        players[player_no].getHand();
    }
    ++player_turn;
    std::string turn = std::to_string(players[player_turn].socket);
    if(player_turn > players.size() - 1){
        turn = "0";
    }
    std::string msgOut;
    for(auto a : players){
        if(a.socket == socket){
            msgOut = "DRAW:" + turn + 
                ":" + a.hand + " " + 
                handValues[a.value];
        }
        else{
            if(str != "0"){
                msgOut = "DRWO:" + turn + 
                    ":" + players[player_no].name + " draws " + 
                    std::to_string(discards.size());
            }
            else{
                msgOut = "DRWO:" + turn + 
                    ":" + players[player_no].name + " stands pat";
            }
        }
        std::cout << a.socket << "<< " << msgOut << "\n";
        send(a.socket, msgOut.c_str(), msgOut.size(), 0);
    }
}
void Game::showHands(int socket){
    std::string msgOut = "SHOW:";
    std::string winner = getWinner();
    for(auto a : players){
        msgOut += a.name + ":" + a.hand + handValues[a.value] + "\n";
    }
    msgOut += "Winner is " + winner + "\n";
    std::cout << socket << "<< " << msgOut << "\n";
    send(socket, msgOut.c_str(), msgOut.size(), 0);
}