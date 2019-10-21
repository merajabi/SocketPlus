# SocketPlus
Thread Safe Socket API based on Acceptor/Connector/Reactor Pattern.
The socket handle ipv4, ipv6, tcp & udp and it resolves names too.
# Sample one: simple client&server with No thread
A simple server code look likes this
```cpp
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

	//Socket::verbose = true;

	{
		Socket s(hostName,hostPort,hostProtocol,hostFamily,true); // the true parameter here make the socket a server
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

```

A simple client code look likes this

```cpp
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

```

## compile & test

```sh
# g++ -Iinclude/ src/*.cpp test/server.cpp -o server.out
# g++ -Iinclude/ src/*.cpp test/client.cpp -o client.out

# ./server.out 8080 tcp ipv4
# ./client.out 127.0.0.1 8080 tcp

# ./server.out 8080 udp ipv6
# ./client.out ::1 8080 udp

```

# Sample two: simple client&server with thread
A simple server code with thread look likes this, we use client part as above
```cpp
#include <iostream>
#include <thread>
#include <string>
#include "socket.h"

void Handel(Socket sp){  // we shall use copy here, the reference may become invalid if we detach the thread!
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
```
## compile & test
compile the server with pthread
```sh
# g++ -pthread -Iinclude/ src/*.cpp test/server.cpp -o server.out
```
# Sample three: simple client&server with thread using smart pointers
we can write the server using smart pointers as follow
```cpp
#include <iostream>
#include <thread>
#include <memory>
#include <string>
#include "socket.h"

void HandelPtr(std::unique_ptr<Socket> sp){
	sp->SetTimeout(2*1000);

	std::string res;
	sp->Recv(res,1000); 
	std::cout << res.size() << std::endl;

	std::string str(10,'y');
	sp->Send(str);
	std::cout << str.size() << std::endl;
}

void Handel(Socket& sp){
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
			std::unique_ptr<Socket> sp(new Socket(s.Listen()));
			if(*sp){	// if this is tcp socket handle connection in new thread
				std::thread t( HandelPtr,std::move(sp) );
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
```
## compile & test
compile the server with pthread
```sh
# g++ -pthread -Iinclude/ src/*.cpp test/server.cpp -o server.out
```
# Sample four: handling multiple servers
we can handle multiple server using Select class, we create a pool of socket objects and listen on all of them, then we can handle each one.
```cpp
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
```
# Sample five: socket server in multiplexing mode
