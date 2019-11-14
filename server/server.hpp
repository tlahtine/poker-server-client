#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <array>
#include <cstdlib>

int getServerSocket(sockaddr_in address);
int getNewClientSocket(sockaddr_in address, int master_socket);
void handleMessage(int sd, std::string msgIn);
void addGame(std::string msgIn, int sd);
#endif