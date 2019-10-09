#ifndef _POSIX_SOCKET_H_
#define _POSIX_SOCKET_H_
/*
typedef uint16_t in_port_t;         
typedef uint32_t in_addr_t;         
 
struct in_addr {
     in_addr_t s_addr;           
 };
 
 struct in6_addr {
     uint8_t s6_addr[16];        
 };
 
 struct sockaddr_in {
     sa_family_t     sin_family; //    short            sin_family;   // e.g. AF_INET
     in_port_t       sin_port;   //    unsigned short   sin_port;     // e.g. htons(3490)
     struct in_addr  sin_addr;   //    struct in_addr   sin_addr;     // see struct in_addr, below
 };								//    char             sin_zero[8];  // zero this if you want to

 struct sockaddr_in6 {
     sa_family_t     sin6_family;    
     in_port_t       sin6_port;      
     uint32_t        sin6_flowinfo;  
     struct in6_addr sin6_addr;      
     uint32_t        sin6_scope_id;  
 };

struct sockaddr {
    unsigned short    sa_family;    // address family, AF_xxx
    char              sa_data[14];  // 14 bytes of protocol address
}; 

struct addrinfo {
    int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
    int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    int              ai_protocol;  // use 0 for "any"
    size_t           ai_addrlen;   // size of ai_addr in bytes
    struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
    char            *ai_canonname; // full canonical hostname

    struct addrinfo *ai_next;      // linked list, next node
};

*/
#include <iostream>
#include <atomic>
#include <string>
#include <vector>
#include <memory>
#include <atomic>


/*
** System header files.
*/
#include <errno.h>        /* errno declaration & error codes.            */
#include <netdb.h>        /* getaddrinfo(3) et al.                       */
#include <net/if.h>       /* if_nametoindex(3).                          */
#include <netinet/in.h>   /* sockaddr_in & sockaddr_in6 definition.      */
#include <stdio.h>        /* printf(3) et al.                            */
#include <stdlib.h>       /* exit(2).                                    */
#include <string.h>       /* String manipulation & memory functions.     */
#include <sys/poll.h>     /* poll(2) and related definitions.            */
#include <sys/socket.h>   /* Socket functions (socket(2), bind(2), etc). */
#include <time.h>         /* time(2) & ctime(3).                         */
#include <unistd.h>       /* getopt(3), read(2), etc.                    */

#include <assert.h>
/*
** Constants & macros.
*/
#define DFLT_HOST			"localhost" // Default server name.
#define DFLT_SERVICE		"daytime"   // Default service name or port.
#define DFLT_PROTOCOL		"tcp"  		// tcp, udp
#define DFLT_FAMILY			"ipv6"   	// AF_INET, AF_INET6, AF_UNSPEC
#define DFLT_SCOPE_ID		"eth0"      // Default scope identifier.

#define INVALID_SOCKET		-1			// Invalid file descriptor.
#define MAXCONNQLEN			3           // Max nbr of connection requests to queue.

/*
** Type definitions (for convenience).
*/
typedef int SOCKET;

/*
** Macro to terminate the program if a system call error occurs.  The system
** call must be one of the usual type that returns -1 on error.  This macro is
** a modified version of a macro authored by Dr. V. Vinge, SDSU Dept. of
** Computer Science (retired)... best professor I ever had.  I hear he writes
** great science fiction in addition to robust code, too.
*/
#define CHK(expr)                                                   \
        do                                                          \
        {                                                           \
           if ( (expr) == -1 )                                      \
           {                                                        \
              fprintf( stderr,                                      \
                       "%s (line %d): System call ERROR - %s.\n",   \
                       "Socket",                                     \
                       __LINE__,                                    \
                       strerror( errno ) );                         \
              exit( 1 );                                            \
           }   /* End IF system call failed. */                     \
        } while ( false )

/*
** This "macro" (even though it's really a function) is loosely based on the
** CHK() macro by Dr. V. Vinge (see server code).  The status parameter is
** a boolean expression indicating the return code from one of the usual system
** calls that returns -1 on error.  If a system call error occurred, an alert
** is written to stderr.  It returns a boolean value indicating success/failure
** of the system call.
**
** Example: if ( !SYSCALL( "write",
**                         count = write( fd, bfr, size ) ) )
**          {
**             // Error processing... but SYSCALL() will have already taken
**             // care of dumping an error alert to stderr.
**          }
*/

bool SYSCALL( const std::string& syscallName, int lineNbr, int status );

void GetDateTime(std::string& dateStr,std::string& timeStr, std::string& msStr, const std::string& datePattern = std::string("%Y/%m/%d"), const std::string& timePattern = std::string("%H:%M:%S"));
std::string GetDateTimeStr();
uint64_t get_ss();

using namespace std;

class socket_guard {
	public:
	typedef enum {RDWR=0,NORD=1,NOWR=2,NORW=3,ERR=7} socket_status; /* 0000 RDWR 0001 NORD 0010 NOWR 0011 NORW 0111 ERR */

	private:
		mutable atomic<int> sock;
		mutable atomic<int> event;
		mutable atomic<socket_status> status;

	public:
		socket_guard(int sock=INVALID_SOCKET):sock(sock),event(0),status(RDWR){
		};
		socket_guard(const socket_guard& sg){
			int oldSock;
			do {
				oldSock = sg.sock; // lock-based operator= inside std::atomic<T>
				sock = oldSock;
			// lock-based function compare_exchange_weak() inside std::atomic<T>
			} while(!sg.sock.compare_exchange_weak(oldSock, INVALID_SOCKET));
			event=sg.event.load();
			status=sg.status.load();
			sg.event=0;
			sg.status=RDWR;
		}
		socket_guard& operator = (const socket_guard& sg){
			assert(sock.load()==INVALID_SOCKET);
			int oldSock;
			do {
				oldSock = sg.sock; // lock-based operator= inside std::atomic<T>
				sock = oldSock;
			// lock-based function compare_exchange_weak() inside std::atomic<T>
			} while(!sg.sock.compare_exchange_weak(oldSock, INVALID_SOCKET));
			event=sg.event.load();
			status=sg.status.load();
			sg.event=0;
			sg.status=RDWR;
			return *this;
		}
		~socket_guard(){
			close();
		}
		bool close(){
			fprintf( stderr,"Socket id: %d Closed.\n",sock.load());
			bool result = false;
			int oldSock;
			do {
				oldSock = sock; // lock-based operator= inside std::atomic<T>
				// lock-based function compare_exchange_weak() inside std::atomic<T>
			} while(oldSock >= 0 && !sock.compare_exchange_weak(oldSock, -1*oldSock));

			if( oldSock >= 0 ){
				result = SYSCALL( "close", __LINE__, ::close( oldSock ) );
				set_status(NORW);
			}
			return result;
		}
		int get() const {
			return (sock.load()<0)?-1*sock.load():sock.load();
		}
		int release(){ // its not a thread safe function
			int tmp=sock;
			reset();
			return tmp;
		}
		void reset(){ sock=INVALID_SOCKET; event=0; status=RDWR;}// its not a thread safe function
		void set_event(int e) { 
			event = e; 
			if( e & (POLLERR|POLLHUP) ){
				set_status(ERR);
			}
			else if( e & POLLRDHUP){
				set_status(NORD);
			}
		}
		int get_event(){return event;}
		void set_status(socket_status s) { status = static_cast<socket_status>(status | s); }
		socket_status get_status(){return status;}
		operator bool () const {return static_cast<bool>( sock.load() >= 0 );}
};

class Socket {
		static std::atomic<unsigned long> sockCount;

		std::shared_ptr<socket_guard>	sockGuard;	// Active Socket
		std::string						host;		// the hostname or IP address (IPv4 or IPv6) of the remote server.
		std::string						service;	// the service name or well-known port number.
		std::string						protocol;	//
		std::string						family;
		std::string						scope;		// For IPv6 sockets only.  
													// This is the index corresponding to the network interface on which to exchange datagrams/streams.
													// This parameter is ignored for IPv4 sockets or when an IPv6 "scoped address" is specified in 'host' 
													// (i.e. where the colon-hex network address is augmented with the scope ID).
		bool							listening;
		unsigned long					timeout;
		struct sockaddr_storage			udpSockStor;

		static bool Initialize();
		static void Finalize();

		bool OpenClient();
		bool OpenServer();

		bool SendTo(const std::string& buffer);
		bool RecvFrom(std::string& buffer, int recvbuflen);
		bool PrintAddrInfo( struct addrinfo *ai );
	public:
		static bool     verbose;       			/* Verbose mode indication.     */

		Socket(const socket_guard& sg=INVALID_SOCKET);
		Socket(const std::string& host, const std::string& service=DFLT_SERVICE, const std::string& protocol=DFLT_PROTOCOL,const std::string& family=DFLT_FAMILY,bool listening=false, unsigned long timeout=0);
		~Socket();
		Socket(const Socket& s);
		Socket& operator = (const Socket& s);

		// its quite possible to call close on one thread and read or write on invalid handel in other thread!!!
		bool Close();
		bool Open(){ return (listening)?OpenServer():OpenClient();}
		bool Send(const std::string& buffer){ return SendTo(buffer);}
		bool Recv(std::string& buffer, int recvbuflen){ return RecvFrom(buffer,recvbuflen);}
		bool ShutDown(int type);

		socket_guard Listen();
		socket_guard Accept();

		int GetFD() const {return sockGuard->get();}
		bool SetTimeout(unsigned long tout);
		bool Listening() const {return listening;}
		const std::string& Protocol() const {return protocol;}
		int GetEvent(){return sockGuard->get_event();}
		void SetEvent(int e);//{sockGuard->SetEvent(e);}
		operator bool () const {return static_cast<bool>(*sockGuard);}
};

#endif //_WIN_SOCKET_H_

