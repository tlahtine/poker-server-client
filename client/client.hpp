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
#include <bits/stdc++.h> 
int players = 0;
int players_in = 0;
int player_turn = 0;
int game_no;
std::string game_name = "Dpokr";
struct Player{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int port = 8888;
    std::string ipAddress = "127.0.0.1";
    std::string name;
    int number;
    bool added = false;
    bool host = false;
    bool cards_dealt = false;
    bool draw = false;
    bool shown = false;
};
#endif