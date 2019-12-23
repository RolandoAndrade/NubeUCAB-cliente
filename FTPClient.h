#include "socket_client.h"
#include "socket_exceptions.h"
#include "FTPRequest.h"
#include "FTPResponse.h"

using namespace std;

class FTPClient
{
	private:
		string host;
		string user;
		string pass;	
		int ip;
		int port;

		int code;
		ClientSocket *controlSocket;
		ClientSocket *dataSocket;
		string request;
		string response;
		FTPResponse ftpResponse;

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

		int pasv()
		{
			request = FTPRequest("PASV").getRequest();
			try
			{
				*controlSocket<<request;
				*controlSocket>>response;
				FTPResponse ftpResponse(response);
				ftpResponse.setResponse(response);
				cout<<ftpResponse.parseResponse(code);
				if(code != 227){
					return code;
				}
					int port = ftpResponse.getPort();
					dataSocket = new ClientSocket(host,port);
			} 
			catch(SocketException &e)
			{
				cout<<"Ha ocurrido un error: "<<e.getMessage()<<endl;
				return -1;
			}
			return code;
		}

		int quit()
		{
			request = FTPRequest("QUIT").getRequest();
			try
			{
				*controlSocket<<request;
				*controlSocket>>response;
				cout<<FTPResponse(response).parseResponse();
			} 
			catch(SocketException &e)
			{
				cout<<"Ha ocurrido un error: "<<e.getMessage()<<endl;
				return 0;
			}
			return 1;
		}

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
		void start()
		{
			cout<<"Conectando al host : "<< host<< " Puerto : "<<port<<endl;
			
			try
			{
				controlSocket = new ClientSocket(host,port);
				*controlSocket>>response;
				cout<<FTPResponse(response).parseResponse();

				request = FTPRequest("USER",user).getRequest();
				*controlSocket<< request;
				*controlSocket>>response;

				request = FTPRequest("PASS",pass).getRequest();
				*controlSocket<<request;
				*controlSocket>>response;

				cout<<FTPResponse(response).parseResponse(code);
				if(code != 230)
				{
					cout<<"Reintroduce el nombre de usuario: ";
					cin>>user;
					cout<<"Reintroduce la contraseña: ";
					pass = getPassword();
					start();
				}
			} 
			catch(SocketException &e)
			{
				cout<<"Ha ocurrido un error: "<<e.getMessage()<<endl;
				return ;
			}
		}

		void communicate();
};