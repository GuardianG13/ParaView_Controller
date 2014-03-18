// File: 		PVC_Socket.h
// Author:		Travis Bueter
// Description:	Class Definition of TCP Sockets

#ifndef PVC_SOCKET_H
#define PVC_SOCKET_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <string>
#include <sstream>
#include <json.h>
#include <pthread.h>
//TODO: Create new json file and include it

using namespace std;

struct helper_args {
	void* this_ptr;
	void* data;
	size_t size;
};

class PVC_Socket
{
public:
	PVC_Socket();

	~PVC_Socket();

	// Description:
	// Connects to socket created by ParaView's MobileSocketPlugin.
	void SocketConnect(int port);

	// Description:
	// Receives data sent over the socket by ParaView.
    int Receive(void* data, int len);
   	
	// Description:
	// Checks to see if ParaView is ready to receive next command. Returns true if ready, otherwise returns false.
	bool receiveReadyCommand();
	
	int join_thread();
	
	// Description:
	// Creates a new thread to send or receive data.
	void pThread_Send(void* data, size_t size);
	
    // Description:
    // Receives camera data from ParaView and updates current camera state.
	void ReceiveData(string &data);
	
	// Description:
	// Prompts ParaView to receive camera data and sends current camera state over socket.
	void* SendData(void* data, size_t size);
	
	// Description:
	// Helper function pointer used when creating threads for sending data.
	static void *SendDataHelper(void *arg);

private:

	int s;
	sockaddr_in svr;
	pthread_t thread;
	bool sending;

};

#include "PVC_Socket.cxx"

#endif // PVC_SOCKET_H
