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

		void get(string args)
		{
			ofstream out(getFileName(args).c_str(), ios::out| ios::binary);
			string data;
			double length;
			// El archivo está disponible
			if(out)
			{
				request =  FTPRequest("TYPE","I").getRequest();
				try
				{
					*controlSocket<<request;
					*controlSocket>>response;
					ftpResponse.setResponse(response);
					cout<<ftpResponse.parseResponse(code);
					if(code != 200)
					{
						return;
					}
				} 
				catch(SocketException &e)
				{
					std::cout<<"Ha ocurrido un error: "<<e.getMessage()<<std::endl;
					return;
				}

				if(pasv()!=227)
				{
					cout<<"No se pudo iniciar el modo PASV"<<endl;
					return;
				}
				
				request =  FTPRequest("RETR",getFileName(args)).getRequest();
				
				try
				{
					*controlSocket<<request;
					*controlSocket>>response;
					cout<<FTPResponse(response).parseResponse(code);
					if(code != 150)
					{
						return;
					}
				} 
				catch(SocketException &e)
				{
					std::cout<<"Un error ha ocurrido: "<<e.getMessage()<<endl;
					return;
				}

				cout<<"Recibiendo el archivo: "<<getFileName(args)<<std::endl;
				
				// store data in buffer named data.
				while (1)
				{
					data = "";
					*dataSocket>>data;
					length = length + data.size();
					if(!data.size())
					{
						break;
					}
					out<<data;
				}

				// close connection.
				(*dataSocket).close();
				*controlSocket>>response;
				out.close();
				int code,precision;
				FTPResponse ftpResponse(response);
				cout<<ftpResponse.parseResponse(code);

				// get file size by status code.
				if(code == 226)
				{
					string size_msg = "bytes";
					precision = 0;

					if(length/1024 >= 1)
					{
						size_msg = "KB";
						length /= 1024;
						precision = 2;

						if(length/1024 >= 1)
						{
							size_msg="MB";
							length /= 1024;

							if(length/1024 >= 1)
							{
								size_msg="GB";
								length /= 1024;
							}
						}
					}

					cout<<setprecision(precision)<<fixed<<"Enviado archivo: "<<getFileName(args)<< " ( " << length <<size_msg<< " )"<<endl;
				}
			}
			else
			{
				cout<<"Archivo: "<<getFileName(args)<<" no pudo transferirse"<<endl;
			}
		}

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
				if(code != 227)
				{
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