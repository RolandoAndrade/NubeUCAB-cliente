#include "FTPClient.h"

int main(int argc, char const *argv[])
{
	if(argc==3)
	{
		string host(argv[1]);
		if(count(host.begin(), host.end(), '.') == 3 && isANumber(argv[2]) && atoi(argv[2]) < 65536)
		{
			int port = atoi(argv[2]);
			string user,pass;
			cout<<"Introduce usuario: ";
			cin>>user;
			cout<<"Introduce contraseña: ";
			pass = getPassword();
			FTPClient client(host, port, user, pass);
			client.start();
			client.communicate();
		}
	}
	return 0;
}