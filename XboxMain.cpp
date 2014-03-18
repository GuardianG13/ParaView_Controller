#include "PVC_Xbox.cxx"

int main(int argc, char** argv)
{
	PVC_Xbox xbox;
	xbox.SocketConnect();
	return xbox.Run(argc,argv);
}