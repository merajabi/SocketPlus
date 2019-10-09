#include <iostream>
#include <string>
#include "socket.h"

bool Recv(Socket& c){
	bool result;
	std::string recvStr;
	result = c.Recv(recvStr,50);
	std::cout << GetDateTimeStr() << " Recv: " << recvStr.size() << ((result)?" OK":" NOK") << std::endl;
	return result;
}
bool Send(Socket& c){
	bool result;
	std::string sendStr(50,'x');
	result = c.Send(sendStr);
	std::cout << GetDateTimeStr() << " Send: "  << sendStr.size() << ((result)?" OK":" NOK") << std::endl;
	return result;
}
int main(int argc, char **argv) {
	std::string peerName="localhost";
	std::string peerPort="9999";
	std::string peerProtocol="tcp";

    printf("usage: %s [server [port [protocol] ] ]   \n", argv[0]);

	if( argc > 1 ){
		peerName = argv[1];
	}
	if( argc > 2 ){
		peerPort = argv[2];
	}
	if( argc > 3 ){
		peerProtocol = argv[3];
	}
	
	Socket::verbose = true;

	{ 
		std::string sendStr(50,'x');
		std::string recvStr;

		Socket c(peerName, peerPort, peerProtocol);
		c.Open();
		sleep(1);

		//c.SetTimeout(5*1000);
		Send(c);
		sleep(1);

		Recv(c);
		sleep(1);

		//c.ShutDown(SHUT_RD); // // peer recv POLLIN | POLLOUT | POLLRDHUP
		//sleep(2);

		Recv(c);
		//sleep(1);

		c.Close();
		sleep(1);

	}

	return 0;
};

