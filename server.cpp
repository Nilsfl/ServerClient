#include <WS2tcpip.h>
#include <string>
#include <fstream>
#include <iostream>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

string getUTC() {
	/*
	* Gets current time and converts it to UTC in specified format
	*/
	time_t t = time(NULL);
	struct tm time;
	char buf[256];
	gmtime_s(&time, &t);
	const char* format = "%Y-%m-%d %X | ";
	strftime(buf, sizeof(time), format, &time);
	return buf;
}

void writeFile(char* msg) {
	/*
	* Creates a file if it does not exist or opens it otherwise. Appends the file with the UTC timestamp and the received message from the client.
	*/
	char filename[] = "server_logs.log";
	fstream file;
	file.open(filename, fstream::app);
	file << getUTC();
	file << msg;
	file << "\n";
	file.close();
}

int main(int argc, char* argv[])
{
	// temp variable for results
	int res = 0;
	// default port
	int port = 54000;

	// Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	res = WSAStartup(ver, &wsData);
	if (res != 0)
	{
		cerr << "Error WSAStartup: " << WSAGetLastError() << endl;
		return 1;
	}

	// Create a socket
	SOCKET sockListen = socket(AF_INET, SOCK_STREAM, 0);
	if (sockListen == SOCKET_ERROR)
	{
		cerr << "Error socket: " << WSAGetLastError() << endl;
		return 1;
	}

	// Connection information
	sockaddr_in info;
	info.sin_family = AF_INET;
	info.sin_port = htons(port);
	info.sin_addr.S_un.S_addr = INADDR_ANY;

	// Bind to socket
	res = bind(sockListen, (sockaddr*)&info, sizeof(info));
	if (res == SOCKET_ERROR)
	{
		cerr << "Error bind: " << WSAGetLastError() << endl;
		return 1;
	}

	// Socket ready for listening
	res = listen(sockListen, SOMAXCONN);
	if (res == SOCKET_ERROR)
	{
		cerr << "Error listen: " << WSAGetLastError() << endl;
		return 1;
	}

	// Client information
	sockaddr_in clientInfo;
	int clientSize = sizeof(clientInfo);

	// Connect with client
	SOCKET clientSock = accept(sockListen, (sockaddr*)&clientInfo, &clientSize);
	if (clientSock == SOCKET_ERROR)
	{
		cerr << "Error accept: " << WSAGetLastError() << endl;
		return 1;
	}
	else
	{
		char host[NI_MAXHOST];
		cout << "Client accepted IP: " << inet_ntop(AF_INET, &clientInfo.sin_addr, host, NI_MAXHOST) << ", Port: " << ntohs(clientInfo.sin_port) << endl;
	}

	// Close listening socket
	closesocket(sockListen);

	// while loop to receive data
	char buf[256];

	while (true)
	{
		ZeroMemory(buf, sizeof(buf));

		// Receive data
		int bytesReceived = recv(clientSock, buf, sizeof(buf), 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv: " << WSAGetLastError() << endl;
			break;
		}

		// Write log file and show sent message in terminal
		writeFile(buf);
		cout << string(buf, 0, bytesReceived) << endl;
	}

	// Close the socket
	closesocket(clientSock);

	// Cleanup winsock
	WSACleanup();
}