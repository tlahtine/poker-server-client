#include "client.hpp"
std::string handleMessage(std::string msgIn, Player& player){
    std::string cmd = msgIn.substr(0, 4);
    std::string msg = msgIn.substr(5);
    std::string rMsg;
    if(cmd == "ADDP"){
        player.name = msg.substr(0, msg.size() - 6);
        players_in = (int)msgIn[msgIn.size() - 1] - 48;
        players = (int)msgIn[msgIn.size() - 3] - 48;
        int number = (int)msgIn[msgIn.size() - 5] - 48;
        player.number = number;
        player.added = true;
        rMsg = "Welcome " + player.name + ". ";
        if(number == 4){
            rMsg += "You are the host. ";
            player.host = true;
        }
        else{
            int players_needed = players - players_in;
            rMsg += std::to_string(players_needed) + " more players needed.";
        }
    }
    if(cmd == "JOIN"){
        ++players_in;
        int players_needed = players - players_in;
        rMsg = msg + " has joined. " + 
                std::to_string(players_needed) + 
                " more players needed.";
    }
    if(cmd == "DEAL" || cmd == "DRAW"){
        rMsg = msg.substr(2);
        player_turn = (int)msg[0] - 48;
    }
    if(cmd == "SHOW"){
        rMsg = msg;
        player.shown = true;
    }
    return rMsg;
}
int main()
{
    mySocket my_socket;
    Player player;
    sockaddr_in hint;
    std::cout << "IP address and port:\n";
    //std::cin >> my_socket.ipAddress >> my_socket.port;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(my_socket.port);
    inet_pton(AF_INET, my_socket.ipAddress.c_str(), &hint.sin_addr);

    int connectRes = connect(my_socket.sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1)
    {
        std::string errMsg = "unable to connect to socket\n";
        throw errMsg;
    }

    char buf[4096];

    while(true) {
        memset(buf, 0, 4096);
        int bytesReceived = recv(my_socket.sock, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            std::cout << "There was an error getting response from server\r\n";
        }
        else
        {
            std::string msgIn = std::string(buf, bytesReceived);
            std::string rMessage = handleMessage(msgIn, player);
            std::cout << rMessage << "\n";
            if(player.cards_dealt && !player.draw && player_turn == player.number){
                std::cout << "Discards (e.g. 124 or 0 to stand pat): ";
                std::string discards;
                std::cin >> discards;
                std::string msgOut = "DRAW:" + discards;
                send(my_socket.sock, msgOut.c_str(), msgOut.size(), 0);
                player.draw = true;
            }
            if(player.host && players == 0){
                std::cout << "Players: ";
                std::cin >> players;
                std::string msgOut = "PLRS:" + std::to_string(players);
                send(my_socket.sock, msgOut.c_str(), msgOut.size(), 0);
            }
            if(players - players_in == 0 && !player.cards_dealt && players != 0){
                player.cards_dealt = true;
                std::string msgOut = "DEAL:";
                send(my_socket.sock, msgOut.c_str(), msgOut.size(), 0);
            }
            if(player.draw && player_turn == 0){
                std::string msgOut = "SHOW:";
                send(my_socket.sock, msgOut.c_str(), msgOut.size(), 0);
            }
            if(player.shown){
                break;
            }
        }
        if(!player.added){
            std::cout << "Name: ";
            std::string name;
            std::cin >> name;
            std::string msgOut = "JOIN:" + name;
            send(my_socket.sock, msgOut.c_str(), msgOut.size(), 0);
        }
    }

    //	Close the socket
    close(my_socket.sock);

    return 0;
}