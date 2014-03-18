// File: 		PVC_Socket.cxx
// Author:		Travis Bueter
// Description:	Class Definition of TCP Sockets

PVC_Socket::PVC_Socket()
{
	s = 0;
	sending = false;
}

PVC_Socket::~PVC_Socket()
{
	close(s);
}

void PVC_Socket::SocketConnect(int port)
{
	svr.sin_family = AF_INET;
	svr.sin_port = htons(port);
	inet_aton("127.0.1.1", &svr.sin_addr);

	s = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == s)
	{
		printf("Socket creation failed. Now exiting. \n");
		exit(1);
	}
	else
		printf("Socket created. \n");

	if(connect(s, (const sockaddr*)&svr, sizeof(sockaddr_in)) == -1)
	{
		printf("Connection failed. Exiting. \n");
		exit(1);
	}
	else
		printf("Connection successful. \n");
}

int PVC_Socket::Receive(void* data, int len)
{
	char* buffer = (char*)data;
	int total = 0;
	do
	{
		int nRecvd = recv(s, buffer+total, len-total, 0);
		if(nRecvd == 0)
		{
			return 0;
		}
		else if(nRecvd == -1)
		{
			printf("Immediate receive Failure\n");
			return -1;
		}
		total += nRecvd;
	}while(total < len);
	return total;
}

bool PVC_Socket::receiveReadyCommand()
{
	int command = 0;
	Receive(&command, 4);
	if(command == 1)
		return true;
	else
		return false;
}

int PVC_Socket::join_thread()
{
	return pthread_join(thread, NULL);
}

void PVC_Socket::pThread_Send(void* data, size_t size)
{
	if(!sending)
	{
		sending = true;
		struct helper_args* args = new helper_args();
		args->this_ptr = this;
		args->data = data;
		args->size = size;
		int rc = pthread_create(&thread, NULL, &PVC_Socket::SendDataHelper, args);
		if (rc)
		{
			cout << "Error: unable to create thread," << rc << endl;
			exit(-1);
		}
	}
}

void PVC_Socket::ReceiveData(string &data)
{
	int ready = 0;
	int command = 2;
	int size;

	while(ready != 1)
	{
		if((size = Receive(&ready, sizeof(ready))) != 4)
			printf("Size not 4 bytes. Continue anyways.\n");
	}

	if(-1 == send(this->s, &command, sizeof(command), 0))
	{
		printf("Command Send Fail. \n");
		exit(1);
	}

	unsigned long long length;
	if(!Receive(&length, sizeof(unsigned long long)))
	{
		printf("Receive Failure\n");
	}

	char metadata[length+1];

	if(!Receive(metadata, length))
	{
		printf("MetaData not received.\n");
	}

	metadata[length] = '\0';
	data = string(metadata);
}

void* PVC_Socket::SendData(void* data, size_t size)
{
	int command = 4;
	if(receiveReadyCommand())
	{
		if(-1 == send(s, &command, sizeof(command), 0))
		{
			printf("Command Send Fail. \n");
			exit(1);
		}

		if(-1 == send(s, data, size, 0))
		{
			printf("Data Send Fail. Size of Data: %lu\n", size);
			exit(1);
		}
	}
	sending = false;
	pthread_exit(NULL);
}

void *PVC_Socket::SendDataHelper(void *arg)
{ 
	struct helper_args* a = (helper_args *)arg;
	((PVC_Socket *)a->this_ptr)->SendData(a->data, a->size); 
	delete a;
}
