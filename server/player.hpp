#ifndef PLAYER_HPP
#define PLAYER_HPP
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
    int socket;
    std::string name;
    std::vector<std::pair<int, std::string>> cards;
    std::string hand;
    void getHand();
    void handValue();
    int value = 0;
    std::array<int, 5> highCards = {0, 0, 0, 0, 0};
};
#endif