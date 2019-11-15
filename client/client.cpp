#include "client.hpp"
int players = 0;
int players_in = 0;
int player_turn = 0;
int game_no;
std::string game_name = "Dpokr";

void handleMessage(std::string msgIn, Player& player){
    std::string cmd = msgIn.substr(0, 4);
    std::string msg = msgIn.substr(5);
    std::string rMsg;
    if(cmd == "ADDP"){
        player.name = msg.substr(0, msg.size() - 8);
        game_no = (int)msgIn[msgIn.size() - 1] - 48;
        players_in = (int)msgIn[msgIn.size() - 3] - 48;
        players = (int)msgIn[msgIn.size() - 5] - 48;
        int number = (int)msgIn[msgIn.size() - 7] - 48;
        player.number = number;
        player.added = true;
        rMsg = "Welcome " + player.name + ". ";
        if(players_in == 1){
            rMsg += "You are the host. ";
            player.host = true;
        }
        else{
            int players_needed = players - players_in;
            rMsg += std::to_string(players_needed) + " more players needed.\n";
            rMsg += "Waiting for players...";
        }
    }
    if(cmd == "JOIN"){
        ++players_in;
        int players_needed = players - players_in;
        rMsg = msg + " has joined. " + 
                std::to_string(players_needed) + 
                " more players needed.";
    }
    if(cmd == "DEAL" || cmd == "DRAW" || cmd == "DRWO"){
        rMsg = msg.substr(2);
        player_turn = (int)msg[0] - 48;
        if(player_turn != player.number && player_turn != 0){
            rMsg += "\nWaiting for other players to draw...";
        }
        if(cmd != "DRWO"){
            std::string cards = msg.substr(2);
            int card_no = 0;
            while(card_no < 5){
                player.cards[card_no].first = cards[0];
                player.cards[card_no].second = cards[1];
                cards.erase(0, 3);
                ++card_no;
            }
            player.handValue = cards;
            player.printHand();
        }
    }
    if(cmd == "SHOW"){
        rMsg = msg;
        player.printHand();
        player.shown = true;
    }
    std::cout << rMsg << std::endl;
}
void newGame(Player& player){
    while(true) {
        char buf[4096];
        memset(buf, 0, 4096);
        int bytesReceived = recv(player.sock, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            std::cout << "There was an error getting response from server\n";
        }
        else
        {
            std::string msgIn = std::string(buf, bytesReceived);
            handleMessage(msgIn, player);
            if(player.cards_dealt && !player.draw && player_turn == player.number){
                std::string discards;
                bool valid = false;
                while(!valid){
                    std::cout << "Discards (e.g. 124 or 0 to stand pat): ";
                    std::cin >> discards;
                    for(int i = 0; i < discards.size(); ++i){
                        if(!isdigit(discards[i]) || discards[i] > '5'){
                            std::cout << "Invalid discards!\n";
                            valid = false;
                        }
                        else{
                            valid = true;
                        }
                    }
                }
                std::string msgOut = "DRAW:" + discards + ":" + std::to_string(game_no);
                send(player.sock, msgOut.c_str(), msgOut.size(), 0);
                player.draw = true;
            }
            if(player.host && players == 0){
                bool valid = false;
                while(!valid){
                    std::cout << "Players (1-5): ";
                    std::cin >> players;
                    if(players >= 1 && players <= 5){
                        valid = true;
                    }
                }
                std::string msgOut = "PLRS:" + 
                    std::to_string(players) + ":" + std::to_string(game_no);
                send(player.sock, msgOut.c_str(), msgOut.size(), 0);
                std::cout << "Waiting for players...\n";
            }
            if(players - players_in == 0 && !player.cards_dealt && players != 0){
                player.cards_dealt = true;
                std::string msgOut = "DEAL:5:" + std::to_string(game_no);
                send(player.sock, msgOut.c_str(), msgOut.size(), 0);
            }
            if(player.draw && player_turn == 0 && !player.shown){
                std::cout << "Press enter to continue: ";
                std::cin.ignore().get();
                std::string msgOut = "SHOW:" + std::to_string(game_no);
                send(player.sock, msgOut.c_str(), msgOut.size(), 0);
            }
            if(player.shown){
                std::cout << "New game? (y/n): ";
                char c;
                std::cin >> c;
                if(player.host){
                    std::string msgOut = "NEWG:" + std::to_string(game_no);
                    send(player.sock, msgOut.c_str(), msgOut.size(), 0);
                }
                if(c == 'y'){
                    system("clear");
                    player.added = false;
                    player.host = false;
                    player.cards_dealt = false;
                    player.draw = false;
                    player.shown = false;
                    std::string msgOut = "JOIN:" + game_name + ":" + player.name;
                    send(player.sock, msgOut.c_str(), msgOut.size(), 0);
                    player.added = true;
                    continue;
                }
                break;
            }
        }
        if(!player.added){
            std::string msgOut = "JOIN:" + game_name + ":" + player.name;
            send(player.sock, msgOut.c_str(), msgOut.size(), 0);
        }
    }
}
int main()
{
    Player player;
    sockaddr_in hint;
    std::cout << "Name: ";
    std::cin >> player.name;

    std::cout << "Server IP address and port:\n";
    std::cin >> player.ipAddress >> player.port;

    hint.sin_family = AF_INET;
    hint.sin_port = htons(player.port);
    inet_pton(AF_INET, player.ipAddress.c_str(), &hint.sin_addr);

    int connectRes = connect(player.sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1)
    {
        std::string errMsg = "unable to connect to socket\n";
        throw errMsg;
    }
    newGame(player);
    //	Close the socket
    close(player.sock);

    return 0;
}