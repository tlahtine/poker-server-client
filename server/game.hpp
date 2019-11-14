#ifndef GAME_HPP
#define GAME_HPP
#include "player.hpp"

const std::array<std::string, 9> handValues = {"High card", "Pair", "Two pairs", "Three of a kind", 
                                        "Straight", "Flush", "Full house", "Four of a kind", 
                                        "Straight flush"};

const std::string cardValues = "23456789TJQKA";
struct Game{
    std::string name;
    int host_socket;
    int players_max = 0;
    int player_turn = 0;
    int cardCount = 0;
    std::vector<std::pair<int, std::string>> deck;
    std::vector<Player> players;
    void newGame(std::string name);
    void shuffleDeck();
    void dealHand(int socket);
    void drawCards(std::string discards, int socket);
    void addPlayer(std::string name, int socket, int game_no);
    void showHands(int socket);
    std::string getWinner();
};
#endif