#include "socket_client.h"
#include "socket_exceptions.h"
#include "FTPRequest.h"
#include "FTPResponse.h"

class FTPClient
{
	private:
		string host;
		string user;
		string pass;	
		int ip;
		int port;

		int return_code;
		ClientSocket *control_socket;
		ClientSocket *data_socket;
		string request;
		string response;
		FTPResponse ftp_response;

		void help();
		void get(string);
		void put(string);

		void _ls(vector<string>, vector<string>, bool print = true);
		void ls(vector<string>, vector<string>, bool print = true);

		string _pwd(bool print = true);	
		string pwd(bool print = true);
		int _cd(string, bool print = true);	
		int cd(string, bool print = true);

		int mkd(string, bool print= false);
		int _mkd(string, bool print= false);
		int pasv();
		bool quit();

	public:
		FTPClient(string shost, int sport, string suser, string spass)
		{
			cout<<"\nNubeUCAB-cliente ha iniciado\n\n";
			host = shost;
			user = suser;
			pass = spass;
			port = sport;
		}

		~FTPClient()
		{

		}
		void start();
		void communicate();
};