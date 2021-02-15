#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main(int argc, char* argv[])
{
	// temp variable to check error codes
	int res;
	// IP-address and port: if not given as arguments set to default values, otherwise set to arguments
	string ip;
	int port;
	if (argc == 2) {
		ip = argv[1];
		port = 54000;
	} 
	else if (argc >= 3) {
		ip = argv[1];
		port = stoi(argv[2]);
	}
	else {
		ip = "127.0.0.1";
		port = 54000;
	}


	// Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	res = WSAStartup(ver, &data);
	if (res != 0)
	{
		cerr << "Error in starting Winsock: " << WSAGetLastError() << endl;
		return 1;
	}

	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Error in socket: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	// Connection information
	sockaddr_in info;
	info.sin_family = AF_INET;
	info.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &info.sin_addr);

	// Connect to server
	res = connect(sock, (sockaddr*)&info, sizeof(info));
	if (res == SOCKET_ERROR)
	{
		cerr << "Error in connecting: " << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	// while loop to send data
	char buf[256];

	while (true)
	{
		// type text in terminal
		cout << "Message: ";
		cin.getline(buf, sizeof(buf));

		// if something is typed
		if (strlen(buf) > 0)
		{
			// send message to server
			int sendResult = send(sock, buf, strlen(buf), 0);
			if (sendResult != SOCKET_ERROR)
			{
				cout << "Message successfully sent" << endl;
			}
			else 
			{
				cout << "Error in sending" << WSAGetLastError() << endl;
				break;
			}
		}
		// if nothing is typed message is displayed to try again
		else 
		{
			cout << "Please type something before sending. Try again" << endl;
		}
	}

	// close everything
	closesocket(sock);
	WSACleanup();
}
