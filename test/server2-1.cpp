#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include "socket.h"
#include "select.h"

void Handle(Socket sp){
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
	//Socket::verbose = true;
	Select pool;										// create a pool
	{
		Socket s1(hostName,"8080","tcp","ipv4",true);	// create each server and add it to pool
		s1.Open();
		pool.Add(s1);

		Socket s2(hostName,"8080","tcp","ipv6",true);
		s2.Open();
		pool.Add(s2);

		Socket s3(hostName,"8080","udp","ipv4",true);
		s3.Open();
		pool.Add(s3);

		Socket s4(hostName,"8080","udp","ipv6",true);
		s4.Open();
		pool.Add(s4);
	}
	{
		std::vector<Socket> selected;				// A vector of selected sockets
		while( pool.Listen(selected) ){				// listen on sockets in the pool
			std::cout << "New network activity." << std::endl;
			for(int i=0; i < selected.size(); i++ ){
				if(selected[i].GetEvent() & POLLIN ){ // if we have POLLIN event
					if( selected[i].Protocol()=="tcp" ){
						Socket ss( selected[i].Accept() ); // we know there is POLLIN event, so no need to listen, just accept the connection
						if(ss){
							// handle the socket send&recv in the same thread or new one
							//Handle(ss);		
							std::thread t( Handle, ss );
							t.join();
						}
					}else{
						// handle the socket send&recv in the same thread or new one
						//Handle( selected[i] );
						std::thread t( Handle,  selected[i] );
						t.join();
					}			
				}
			}
			selected.clear();		// clear list of selected sockets
		}
	}
	return 0;
};
