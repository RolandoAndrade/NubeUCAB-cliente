#include "socket.h"
#include "socket_exceptions.h"

class ClientSocket : private Socket
{
	public:
		ClientSocket();
		ClientSocket(int, int);
		ClientSocket(std::string, int);
		~ClientSocket();
		ClientSocket& operator << (std::string&);
		ClientSocket& operator >> (std::string&);
		void close();
		int getFD();
};