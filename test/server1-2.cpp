#include <iostream>
#include <thread>
#include <string>
#include "socket.h"

void Handel(Socket sp){
	sp.SetTimeout(2*1000);

	std::string res;
	sp.Recv(res,1000);
	std::cerr << res.size() << std::endl;

	std::string str(10,'y');
	sp.Send(str);
	std::cerr << str.size() << std::endl;
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

	//Socket::verbose = true;

	{
		Socket s(hostName,hostPort,hostProtocol,hostFamily,true);

		if(s.Open()){
			Socket ss( s.Listen() );
			if(ss){	// if this is tcp socket handle connection in new thread
				std::thread t( Handel, ss );
				t.detach(); //join or detach
			}else{		// if this is udp socket send&recv in original thread
				Handel(s);
			}			
		}
		//s.Close();
		sleep(5);	// we wait here for new thread to finish, because we have detached it, we donot want to exit main process while any thread is still running
	}
	return 0;
};
