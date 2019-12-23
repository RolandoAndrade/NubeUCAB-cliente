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

			int ip = lookup(host);
			if(!Socket::connect(ip,port))
			{
				throw SocketException(strerror(errno));
			}
		}
		~ClientSocket()
		{
			Socket::close();
		}

		ClientSocket& operator << (string &s)
		{
			if(Socket::send(s)==-1)
			{
				throw SocketException(strerror(errno));
			}
			return *this;
		}

		ClientSocket& operator >> (string &s)
		{
			if(Socket::recv(s)==-1)
			{
				throw SocketException(strerror(errno));
			}
			return *this;
		}

		void close()
		{
			if(!Socket::close())
			{
				throw SocketException(strerror(errno));
			}
		}

		int getFD()
		{
			if(!Socket::close())
			{
				throw SocketException(strerror(errno));
			}			
		}
};