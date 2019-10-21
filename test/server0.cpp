#include <iostream>
#include <string>
#include "socket.h"

void Handel(Socket& sp){ // we can use reference or copy without any problem, 
						//	socket objects knows this is a copy and will not close descriptor if another object referencing it
	sp.SetTimeout(2*1000);

	std::string res;
	sp.Recv(res,1000); 
	std::cout << res.size() << std::endl;

	std::string str(10,'y');
	sp.Send(str);
	std::cout << str.size() << std::endl;
}

int main(int argc, char **argv) {
	std::string hostName="localhost";
	std::string hostPort="8080";
	std::string hostProtocol="tcp";
	std::string hostFamily="ipv4";

    std::cout << "usage: "<< argv[0] << " [port [protocol [family] ] ] ] " << std::endl;
    std::cout << "defaults: "<< "./server.out 8080 tcp ipv4 " << std::endl;
	if( argc > 1 ){
		hostPort = argv[1];
	}
	if( argc > 2 ){
		hostProtocol = argv[2];
	}
	if( argc > 3 ){
		hostFamily = argv[3];
	}

	Socket::verbose = true;

	{
		Socket s(DFLT_HOST,hostPort,hostProtocol,hostFamily,true); // the true parameter here make the socket a server
		if(s.Open()){
			Socket ss( s.Listen() ); // listen on server socket, and construct new socket
			if( hostProtocol=="tcp" && ss ){	// if ss is valid socket the new socket is created so this is a tcp socket
												// still we should be carefull because we have not handled exceptions yet!
				Handel(ss);
			}else if (hostProtocol=="udp") {	// if ss is invalid this is a udp socket we should send&recv via main socket
				Handel(s);
			}			
		}
		//s.Close();	// close is not mandatory here, 
						// the socket object will close the descriptor 
						// when the last object referencing this socket descriptor is destructed
	}
	return 0;
};

