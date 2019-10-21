#include <iostream>
#include <string>
#include "socket.h"

int main(int argc, char **argv) {
	std::string peerName="localhost";
	std::string peerPort="8080";
	std::string peerProtocol="tcp";

    std::cout << "usage: "<< argv[0] << " [server [port [protocol] ] ] " << std::endl;
    std::cout << "defaults: "<< "./client.out 127.0.0.1 8080 tcp " << std::endl;

	if( argc > 1 ){
		peerName = argv[1];
	}
	if( argc > 2 ){
		peerPort = argv[2];
	}
	if( argc > 3 ){
		peerProtocol = argv[3];
	}
	
	//Socket::verbose = true;

	{
		Socket c(peerName, peerPort, peerProtocol);
		c.Open();
		c.SetTimeout(5*1000);

		std::string str(1000,'x');
		c.Send(str);
		std::cout << str.size() << std::endl;

		std::string res;
		c.Recv(res,10);
		std::cout << res.size() << std::endl;
	}
	return 0;
};

