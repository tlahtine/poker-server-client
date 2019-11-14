#include "server.hpp"
std::array<int, 100> client_socket;
int main(){
    for(int i = 0; i < client_socket.size(); ++i){
        client_socket[i] = 0;
    }
    sockaddr_in address;
    fd_set readfds;
    int master_socket, sd;
    try{
        master_socket = getServerSocket(address);
        while(true){
            FD_ZERO(&readfds); 
            FD_SET(master_socket, &readfds);
            int max_sd = master_socket;
            for (int i = 0; i < client_socket.size(); i++)
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
                for(int i = 0; i < client_socket.size(); ++i){
                    if(client_socket[i] == 0){
                        client_socket[i] = new_socket;
                        break;
                    }
                }
            }
            for (int i = 0; i < client_socket.size(); i++) 
            { 
                if(client_socket[i] > 0){
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
    }
    catch(std::string msg){
        std::cout << msg << "\n";
        return -1;
    }
}
