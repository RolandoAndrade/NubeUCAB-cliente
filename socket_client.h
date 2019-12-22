#include "socket.h"
#include "socket_exceptions.h"
#include "commands.h"

class ClientSocket : private Socket
{
	public:
		ClientSocket()
		{

		}
		ClientSocket(int host, int port)
		{
			if(!Socket::create())
			{
				throw SocketException(strerror(errno));
			}
			if(!Socket::connect(host,port))
			{
				throw SocketException(strerror(errno));
			}
		}

		ClientSocket(string host, int port)
		{
			if(!Socket::create())
			{
				throw SocketException(strerror(errno));
			}
			int ip = lookup(ip_address);
			if(!Socket::connect(ip,port)){
				throw SocketException(strerror(errno));
			}
		}
		~ClientSocket()
		{
			Socket::close();
		}
		
		ClientSocket& operator << (std::string&);
		ClientSocket& operator >> (std::string&);
		void close();
		int getFD();
};