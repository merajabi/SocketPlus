#ifndef _POSIX_SOCKET_CONFIG_H_
#define _POSIX_SOCKET_CONFIG_H_

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
//#define XSTR(x) STR(x)
//#define STR(x) #x

#if defined(__unix__) || defined(__linux__)
	#define OS_TYPE_NIX

#elif defined(_WIN32) || defined(_WIN64)
	#define OS_TYPE_WIN
	#define WIN32_LEAN_AND_MEAN
	//#pragma message "The value of ABC: " XSTR(_WIN32_WINNT)
	#undef _WIN32_WINNT
	#define _WIN32_WINNT 0x0600
#endif 

/*
** System header files.
*/
//#include <errno.h>        /* errno declaration & error codes.            */
//#include <stdio.h>        /* printf(3) et al.                            */
//#include <stdlib.h>       /* exit(2).                                    */
#include <string.h>       /* String manipulation & memory functions.     */
#include <time.h>         /* time(2) & ctime(3).                         */
#include <unistd.h>       /* getopt(3), read(2), etc.                    */
#include <assert.h>

#ifdef OS_TYPE_NIX
	#include <netdb.h>        /* getaddrinfo(3) et al.                       */
	#include <net/if.h>       /* if_nametoindex(3).                          */
	#include <netinet/in.h>   /* sockaddr_in & sockaddr_in6 definition.      */
	#include <sys/poll.h>     /* poll(2) and related definitions.            */
	#include <sys/socket.h>   /* Socket functions (socket(2), bind(2), etc). */

	#define SOCKET_FUNC_CLOSE		::close
	#define SOCKET_FUNC_POLL		poll
	#define GET_LAST_ERROR_NUMBER 	errno
	#define ERROR_INTERRUPT			EINTR					// A signal occurred before any requested event;
	#define ERROR_WOULDBLOCK		EAGAIN // EWOULDBLOCK) 	// The socket is marked nonblocking and the receive operation would block, or a receive timeout had been set and the timeout expired before data was received.
	#define ERROR_BADFD				EBADF

	#define INVALID_SOCKET			-1						// Invalid file descriptor.
  	#define MAKE_ADDRINFO(ai_flags,ai_family,ai_socktype,ai_protocol,ai_addrlen,ai_addr,ai_canonname,ai_next) {ai_flags,ai_family,ai_socktype,ai_protocol,ai_addrlen,ai_addr,ai_canonname,ai_next}
	typedef int32_t SOCKET;		// Type definitions (for convenience).

#elif defined(OS_TYPE_WIN)
	#undef UNICODE

	#include <winsock2.h>
	#include <windows.h>
	#include <winsock.h>
	#include <ws2tcpip.h>
	#include <iphlpapi.h>
 
	#define SOCKET_FUNC_CLOSE		closesocket
	#define SOCKET_FUNC_POLL		WSAPoll
	#define GET_LAST_ERROR_NUMBER 	WSAGetLastError()
	#define ERROR_INTERRUPT			WSAEINTR			// Interrupted function call. A blocking operation was interrupted by a call to WSACancelBlockingCall.
	#define ERROR_WOULDBLOCK		WSAEWOULDBLOCK 		// Resource temporarily unavailable. This error is returned from operations on nonblocking sockets that cannot be completed immediately
	#define ERROR_BADFD				WSAENOTSOCK

	#define MSG_NOSIGNAL			0
	#define POLLRDHUP				POLLHUP
	#define SHUT_RD					SD_RECEIVE
	#define SHUT_WR					SD_SEND
	#define SHUT_RDWR				SD_BOTH
  	#define MAKE_ADDRINFO(ai_flags,ai_family,ai_socktype,ai_protocol,ai_addrlen,ai_addr,ai_canonname,ai_next) {ai_flags,ai_family,ai_socktype,ai_protocol,(size_t)ai_addrlen,ai_canonname,ai_addr,ai_next}
	// typedef size_t socklen_t;
	// Need to link with Ws2_32.lib
	//#pragma comment (lib, "Ws2_32.lib")
	//#pragma comment (lib, "Mswsock.lib")
	//#pragma comment (lib, "AdvApi32.lib")

#endif

#endif //_POSIX_SOCKET_CONFIG_H_

