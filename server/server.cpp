#include "server.hpp"
#include "game.hpp"
std::vector<Game> games;

int getServerSocket(sockaddr_in address){
    int port = 8888;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    int master_socket = socket(AF_INET , SOCK_STREAM , 0);
    if(master_socket == 0){
        std::string errMsg = "Socket failed";
        throw errMsg;
    }
    int status = bind(master_socket, (struct sockaddr *)&address, sizeof(address));
	if (status < 0) 
	{ 
        std::string errMsg = "Failed to bind socket to local address";
        throw errMsg;
	}
    std::cout << "Listening to port " << port <<"\n"; 
    listen(master_socket, 3); //maximum of 3 pending connections
	std::cout << "Waiting for connections ...\n";
    return master_socket;
}
int getNewClientSocket(sockaddr_in address, int master_socket){
    int addrlen = sizeof(address);
    int new_socket = accept(master_socket, (struct sockaddr *)&address,
                    (socklen_t*)&addrlen);
    
    //inform user of socket number - used in send and receive commands 
    std::cout << "New connection, socket fd is " << new_socket << 
                ", ip is " <<  inet_ntoa(address.sin_addr) <<
                ", port: " << ntohs(address.sin_port) << "\n";
    std::string message = "connected";
    send(new_socket, message.c_str(), message.size() + 1, 0);
    return new_socket;
}
void newGame(std::string msg, int sd){
    Game game;
    game.name = msg.substr(0, 5);
    game.host_socket = sd;
    game.players_max = 0;
    game.player_turn = 0;
    game.players.clear();
    game.shuffleDeck();
    games.emplace_back(game);
}
void handleMessage(int sd, std::string msgIn){
    std::string cmd = msgIn.substr(0, 4);
    std::string msg = msgIn.substr(5);
    std::cout << sd << ">> " << msgIn << "\n";
    int game_number = -1;
    if(cmd == "JOIN"){
        if(games.size() == 0){
            newGame(msg, sd);
            game_number = 0;
        }
        else{
            for(int i = 0; i < games.size(); ++i){
                if(msg.substr(0, 5) == games[i].name){
                    if(games[i].players_max > games[i].players.size()){
                        game_number = i;
                        break;
                    }
                }
            }
        }
        if(game_number == -1){
            newGame(msg, sd);
            game_number = games.size() - 1;
        }
    }
    else{
        game_number = (int)msgIn[msgIn.size() - 1] - 48;
        msg = msg.substr(0, msg.size() - 2);
    }
    if(cmd == "JOIN"){
        std::string name = msg.substr(6, msg.size() - 6);
        games[game_number].addPlayer(name, sd, game_number);
    }
    if(cmd == "DEAL"){
        games[game_number].cardCount = atoi(msg.c_str());
        games[game_number].dealHand(sd);
    }
    if(cmd == "PLRS"){
        games[game_number].players_max = (int)msgIn[msgIn.size() - 3] - 48;
    }
    if(cmd == "DRAW"){
        std::string discards = msgIn.substr(5);
        int dlen = discards.size() - 2;
        discards = discards.substr(0, dlen);
        games[game_number].drawCards(discards, sd);
    }
    if(cmd == "SHOW"){
        games[game_number].showHands(sd);
    }
    if(cmd == "NEWG"){
        games[game_number].game_on = false;
        bool games_on = false;
        for(Game game : games){
            if(game.game_on){
                games_on = true;
                break;
            }
        }
        if(!games_on){
            games.clear();
        }
    }
}
