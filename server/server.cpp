#include "server.hpp"
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
std::string getWinner(){
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
void handleMessage(int sd, std::string msgIn){
    Player player;
    std::string cmd = msgIn.substr(0, 4);
    std::string msgOut;
    if(cmd == "JOIN"){
        std::string name = msgIn.substr(5);
        for(int i = 0; i < client_socket.size(); ++i){
            if(sd == client_socket[i]){
                player.name = name;
                player.number = sd;
                players.emplace_back(player);
                msgOut = "ADDP:" + name + ":" + std::to_string(sd) + 
                         ":" + std::to_string(players_max) + 
                         ":" + std::to_string(players.size());
            }
            else{
                msgOut = msgIn;
            }
            send(client_socket[i], msgOut.c_str(), msgOut.size(), 0);
        }
    }
    if(cmd == "DEAL"){
        players[sd - 4].dealHand(5);
        players[sd - 4].handValue();
        players[sd - 4].getHand();

        msgOut = "DEAL:" + std::to_string(players[player_turn].number) + ":" + 
            players[sd - 4].hand + " " + handValues[players[sd - 4].value];
        
        send(sd, msgOut.c_str(), msgOut.size(), 0);
    }
    if(cmd == "PLRS"){
        players_max = (int)msgIn[msgIn.size() - 1] - 48;
    }
    if(cmd == "DRAW"){
        std::string discards = msgIn.substr(5);
        if(discards != "0"){
            players[sd - 4].drawCards(discards);
        }
        players[sd - 4].handValue();
        players[sd - 4].getHand();
        ++player_turn;
        for(int i = 0; i < client_socket.size(); ++i){
            if(sd == client_socket[i]){
                msgOut = "DRAW:" + std::to_string(players[player_turn].number) + 
                    ":" + players[sd - 4].hand + " " + 
                    handValues[players[sd - 4].value];
            }
            else{
                if(discards != "0"){
                    msgOut = "DRAW:" + std::to_string(players[player_turn].number) + 
                        ":" + players[sd - 4].name + " draws " + 
                        std::to_string(discards.size());
                }
                else{
                    msgOut = "DRAW:" + std::to_string(players[player_turn].number) + 
                        ":" + players[sd - 4].name + " stands pat";
                }
            }
            send(client_socket[i], msgOut.c_str(), msgOut.size(), 0);
        }
    }
    if(cmd == "SHOW"){
        msgOut = "SHOW:";
        std::string winner = getWinner();
        for(auto a : players){
            msgOut += a.name + ":" + a.hand + " " + handValues[a.value] + "\n";
        }
        msgOut += "Winner is " + winner + "\n";
        send(sd, msgOut.c_str(), msgOut.size(), 0);
    }
}
int main(){
    sockaddr_in address;
    fd_set readfds;
    int master_socket, sd;
    shuffleDeck();
    try{
        master_socket = getServerSocket(address);
        while(true){
            FD_ZERO(&readfds); 
            FD_SET(master_socket, &readfds);
            int max_sd = master_socket;
            for (int i = 0 ; i < client_socket.size() ; i++) 
            { 
			    sd = client_socket[i]; 
                if(sd > 0){
				    FD_SET(sd , &readfds); 
                }
                if(sd > max_sd){
                    max_sd = sd;
                }
            }
            //wait for an activity on one of the sockets, timeout is NULL , 
            //so wait indefinitely 
            int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL); 
            //incoming connection
            if(FD_ISSET(master_socket, &readfds)) 
            {
                int new_socket = getNewClientSocket(address, master_socket);
                client_socket.emplace_back(new_socket);
            }
            for (int i = 0; i < client_socket.size(); i++) 
            { 
                sd = client_socket[i];
                if(FD_ISSET(sd, &readfds)) 
                {
                    char buffer[1025];
                    ssize_t valread = read(sd, buffer, 1024);
                    if (valread == 0)
                    {
                        //client disconnected
                        int addrlen = sizeof(address);
                        getpeername(sd , (struct sockaddr*)&address, (socklen_t*)&addrlen); 
                        std::cout << "Host disconnected, ip " << 
                        inet_ntoa(address.sin_addr) << ", port " <<
                        ntohs(address.sin_port) << "\n"; 
                            
                        close(sd);
					    client_socket[i] = 0;
                    } 
                        
                    else
                    {
                        handleMessage(sd, std::string(buffer, 0, valread));
                    } 
                } 
            }
        }
    }
    catch(std::string msg){
        std::cout << msg << "\n";
        return -1;
    }
}
