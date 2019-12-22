#include "socket_client.h"
#include "socket_exceptions.h"
#include "FTPRequest.h"
#include "FTPResponse.h"

class FTPClient
{
	private:
		std::string host;
		std::string user;
		std::string passwd;	
		int ip;
		int port;

		int return_code;
		ClientSocket *control_socket;
		ClientSocket *data_socket;
		std::string request;
		std::string response;
		FTPResponse ftp_response;

		void help();
		void get(std::string);
		void put(std::string);

		void _ls(std::vector<std::string>, std::vector<std::string>, bool print = true);
		void ls(std::vector<std::string>, std::vector<std::string>, bool print = true);

		std::string _pwd(bool print = true);	
		std::string pwd(bool print = true);
		int _cd(std::string, bool print = true);	
		int cd(std::string, bool print = true);

		int mkd(std::string, bool print= false);
		int _mkd(std::string, bool print= false);
		int pasv();
		bool quit();

	public:
		FTPClient(std::string, int, std::string, std::string);
		~FTPClient();
		void start();
		void communicate();
};