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

		void help()
		{

		}

		void get(string args)
		{
			ofstream out(getFileName(args).c_str(), ios::out| ios::binary);
			string data;
			double length;
			// El archivo está disponible
			if(out)
			{
				request =  FTPRequest("TYPE","B").getRequest();
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
					cout<<"Ha ocurrido un error: "<<e.getMessage()<<endl;
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
					cout<<"Un error ha ocurrido: "<<e.getMessage()<<endl;
					return;
				}

				cout<<"Recibiendo el archivo: "<<getFileName(args)<<endl;
				
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

				(*dataSocket).close();
				*controlSocket>>response;
				out.close();
				int code,precision;
				FTPResponse ftpResponse(response);
				cout<<ftpResponse.parseResponse(code);

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

		void put(string args)
		{
			ifstream in(args.c_str(), ios::in | ios::binary | ios::ate);
			// Si existe el archivo
			if(in)
			{
				long length = in.tellg();
				in.seekg (0, in.beg);
				request =  FTPRequest("TYPE","B").getRequest();

				try
				{
					*controlSocket<<request;
					*controlSocket>>response;
					ftpResponse.setResponse(response);
					cout<<ftpResponse.parseResponse(code);
					if(code != 200){
						return;
					}
				} catch(SocketException &e){
					cout<<"Ha ocurrido un error: "<<e.getMessage()<<endl;
					return;
				}

				if(pasv()!=227)
				{
					cout<<"No se ha podido iniciar la transferencia del archivo"<<endl;
					return;
				}
				
				request =  FTPRequest("STOR",getFileName(args)).getRequest();
				try
				{
					*controlSocket<<request;
					*controlSocket>>response;
					ftpResponse.setResponse(response);
					cout<<ftpResponse.parseResponse(code);
					if(code != 150)
					{
						return;
					}
				} 
				catch(SocketException &e)
				{
					cout<<"Ha ocurrido un error: "<<e.getMessage()<<endl;
					return;
				}

				cout<<"Enviando el archivo "<<getFileName(args)<<endl;
				string data;

				double c_length=length;
				while (length>0)
				{
					int read_sz = Socket::MAXRECV<length ? Socket::MAXRECV : length;
					char buf[Socket::MAXRECV+1];
					in.read(buf,read_sz);
					data.assign(buf,read_sz);
					*dataSocket<<data;
					length -= read_sz;
				}

				(*dataSocket).close();
				*controlSocket>>response;
				in.close();
				int code,precision;
				FTPResponse ftp_response(response);
				cout<<ftp_response.parseResponse(code);
				
				if(code == 226)
				{
					string size_msg = "bytes";
					precision = 0;

					if(c_length/1024 >= 1)
					{
						size_msg = "KB";
						c_length /= 1024;
						precision = 2;

						if(c_length/1024 >= 1)
						{
							size_msg="MB";
							c_length /= 1024;

							if(c_length/1024 >= 1)
							{
								size_msg="GB";
								c_length /= 1024;
							}
						}
					}
					cout<<setprecision(precision)<<fixed<<"Se ha transferido el archivo "<<getFileName(args)<< " ( " << c_length <<size_msg<< " )"<<endl;
				}
			}
			else
			{
				cout<<"El archivo "<<getFileName(args)<<" no existe"<<endl;
			}
		}

		void _ls(vector<string>, vector<string>, bool print = true)
		{

		}

		void ls(vector<string> flags, vector<string> args, int print = 1)
		{
			request = FTPRequest("LIST",flags,args).getRequest();
			
			try
			{
				*controlSocket<<request;
				*controlSocket>>response;
				ftpResponse.setResponse(response);
				string p_response= ftpResponse.parseResponse(code);
				
				if(print)
				{
					cout<<p_response;
				}

				if(code != 150)
				{
					response = "";
					return;
				}

				//Obtener respuesta
				while(1)
				{
					response = "";
					*dataSocket >> response;
					if(!response.size())
					{
						break;
					}
					if(print)
					{
						cout<<response;
					}																				
				}

				(*controlSocket)>>response;
				ftpResponse.setResponse(response);
				p_response= ftpResponse.parseResponse(code);		
				if(print)
				{
					cout<<p_response;
				}
			} 
			catch(SocketException &e)
			{
				cout<<"Ha ocurrido un error: "<<e.getMessage()<<endl;
				return ;
			}
		}

		string _pwd(bool print = true)
		{
			return "";
		}

		string pwd(int print = 1)
		{
			request = FTPRequest("PWD","").getRequest();
			try
			{
				*controlSocket<<request;
				*controlSocket>>response;
				ftpResponse.setResponse(response);		
				string p_response = ftpResponse.parseResponse(code);
				if(print)
				{
					cout<<p_response;
				}
				return p_response.substr(1,p_response.length()-4);
			} 
			catch(SocketException &e)
			{
				cout<<"Ha ocurrido un error: "<<e.getMessage()<<endl;
				return "" ;
			}
		}

		int _cd(string args, int print = 1)
		{
			response = execute("cd",args,code);
			if(print)
			{
				cout<<response;
			}
			return code;
		}

		int cd(string args, int print = 1)
		{
			request = FTPRequest("CWD",args).getRequest();
			// socket exceptions handling
			try
			{
				*controlSocket<<request;
				*controlSocket>>response;
				ftpResponse.setResponse(response);
				if(print)
				{
					cout<<ftpResponse.parseResponse(code);
				}
				return ftpResponse.returnCode();
			} 
			catch(SocketException &e)
			{
				cout<<"Ha ocurrido un error: "<<e.getMessage()<<endl;
				return -1;
			}
		}

		int rename(string args, int print = 1)
		{
			request = FTPRequest("RENM",args).getRequest();
			// socket exceptions handling
			try
			{
				*controlSocket<<request;
				*controlSocket>>response;
				ftpResponse.setResponse(response);
				if(print)
				{
					cout<<ftpResponse.parseResponse(code);
				}
				return ftpResponse.returnCode();
			} 
			catch(SocketException &e)
			{
				cout<<"Ha ocurrido un error: "<<e.getMessage()<<endl;
				return -1;
			}
		}

		int rm(string args, int print = 1)
		{
			request = FTPRequest("REMV",args).getRequest();
			// socket exceptions handling
			try
			{
				*controlSocket<<request;
				*controlSocket>>response;
				ftpResponse.setResponse(response);
				if(print)
				{
					cout<<ftpResponse.parseResponse(code);
				}
				return ftpResponse.returnCode();
			} 
			catch(SocketException &e)
			{
				cout<<"Ha ocurrido un error: "<<e.getMessage()<<endl;
				return -1;
			}
		}

		int mkd(string args, int print = 0)
		{
			request = FTPRequest("MKD",args).getRequest();
			// socket exceptions handling
			try
			{
				*controlSocket<<request;
				*controlSocket>>response;
				ftpResponse.setResponse(response);
				if(print)
				{
					cout<<ftpResponse.parseResponse(code);
				}
				return ftpResponse.returnCode();
			} 
			catch(SocketException &e)
			{
				cout<<"Exception occurred : "<<e.getMessage()<<endl;
				return -1;
			}
		}

		int _mkd(string, bool print= false)
		{
			return 0;
		}

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

				cout<<response<<endl;

				request = FTPRequest("USER",user).getRequest();
				*controlSocket<< request;
				*controlSocket>>response;

				cout<<request<<" "<<response<<endl;

				request = FTPRequest("PASS",pass).getRequest();
				*controlSocket<<request;
				*controlSocket>>response;

				cout<<request<<" "<<response<<endl;

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

		void communicate()
		{
			string command,cmd;
			vector<string> flags,args;

			while(1)
			{
				flags.clear();
				args.clear();
				cout<<"NubeUCAB> ";
				getline(cin,command);

				if(parseCommand(command,cmd,flags,args))
				{
					
					if(cmd=="get" && (args.size() == 1 || args.size()==2) && flags.size()==0)
					{
						string curr_loc = _pwd(0);
						string curr_loc_server = pwd(0);

						if(args.size()==2)
						{
							if(_cd(args[1],0)!= 1)
							{
								cout<<"El destino no existe"<<endl;
								continue;
							}
						}

						string filePath = getFilePath(args[0]);

						if(filePath!="")
						{
							if(cd(filePath,0) != 250)
							{
								_cd(curr_loc,0);
								cout<<"El destino no existe"<<endl;
								continue;
							}
						}

						get(getFileName(args[0]));
						cd(curr_loc_server,0);
						_cd(curr_loc,0);
					} 
					
					else if(cmd=="put" && (args.size() == 1 || args.size()==2) && flags.size()==0)
					{
						string curr_loc = pwd(0);

						if(args.size()==2)
						{
							if(cd(args[1],0)!= 250)
							{
								cout<<"El destino no existe"<<endl;
								continue;
							}
						}

						put(args[0]);
						cd(curr_loc,0);
					}
					else if(cmd=="pwd" && !args.size()&& !flags.size())
					{
						pwd();
					}
					
					else if(cmd=="cd" && !flags.size()&& args.size() == 1)
					{
						cd(args[0]);
					}
					else if(cmd=="rm" && !flags.size()&& args.size() == 1)
					{
						rm(args[0]);
					}
					else if(cmd=="rename" && !flags.size()&& args.size() == 2)
					{
						rename(args[0]+" "+args[1]);
					}
					else if(cmd=="ls")
					{			
						if(pasv()!=227)
						{
							cout<<"No se pueden listar los archivos"<<endl;
							continue;
						}
						ls(flags,args);
					}
					else if(cmd=="mkdir" && args.size() == 1 && !flags.size())
					{
						int flag = 1;
						string curr_loc = pwd(0);
						vector<string> dirs = tokenize(args[0],"/");

						for(int i=0;i<dirs.size();i++)
						{
							if(mkd(dirs[i],0)!=257 && cd(dirs[i],0) != 250)
							{
								cout<<"No se pudo crear el directorio"<<endl;
								flag = 0;
								break;
							}				
						}

						cd(curr_loc,0);

						if(flag)
						{
							cout<<"Directorio "<<args[0]<< " creado"<<endl;
						}
					}
			
					else if(cmd=="!pwd" && args.size() == 0 && flags.size()==0)
					{
						_pwd();
					}
					else if(cmd=="!cd" && flags.size() == 0 && args.size() == 1)
					{
						_cd(args[0]);
					}
					else if(cmd=="!ls")
					{
						_ls(flags,args);
					}
					else if(cmd=="!mkdir" && args.size() == 1 && !flags.size())
					{
						bool flag = 1;
						string curr_loc = _pwd(0);

						vector<string> dirs = tokenize(args[0],"/");
						for(int i=0;i<dirs.size();i++)
						{
							int status = _mkd(dirs[i],0);
							status = status | _cd(dirs[i],0);
							if(_mkd(dirs[i],0)!=1 && _cd(dirs[i],0) != 1)
							{
								cout<<"No se ha podido crear directorio"<<endl;
								flag = 0;
								break;
							}				
						}

						_cd(curr_loc,false);
						if(flag)
						{
							cout<<"Directorio "<<args[0]<< " creado"<<endl;
						}
					}
					else if(cmd=="quit")
					{
						if(quit())
						{
							(*controlSocket).close();
							return;
						}
						else
						{
							cout<<"No se puede terminar la sesión"<<endl;
						}
					}
					else if(cmd=="help")
					{
						help();
					}
					else
					{
						cout<<"No existe el comando"<<endl;
					}
				}
			}
		}
};