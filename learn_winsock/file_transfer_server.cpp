


#include "stdafx.h"
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
//#include "fileMessage.h"
#include "direct.h"
#include "string.h"
#include <cstdlib>
#include <fstream>
#include <assert.h>
#include <crtdbg.h>
#include <stdio.h>

#include <WinBase.h>
// define listenning port of Server
#define PORT 65432

#pragma comment(lib, "Ws2_32.lib")

using namespace std;
// store the cmd from keyboard to quit
char Command[20];
// store the file name with infomation
char filename[128];
// store the file name that will be sent to client without path
char fname[128];
// define the file input stream
ifstream inFile;
// define buffer to store file buffer
char fileBuffer[1000];
// declare the recv cmd from keyboard thread function
unsigned int _stdcall GetCommand(void *par);
// declare the send file thread function
unsigned int _stdcall SendFile(void *par);


// file transfer thread function

//#if 0
int main(int argc, char *argv[])
{
	// define socket variable
	// define server socket
	SOCKET sock_server;
	// store client address
	struct sockaddr_in addr, client_addr;
	HANDLE hThread1, hThread2;
	unsigned long ThreadId1, ThreadId2;
	int addr_len = sizeof(struct sockaddr_in);
	cout << "Input the file name that you want " 
		<< endl  << "to send:(name length less than 128 bytes)" << endl;
	cin >> filename;
	// open the file which will be dispatched in binary format.
	inFile.open(filename, ios::in | ios::binary);
	if (!inFile.is_open()) {
		// if can not open the file, then exit
		cout << "Cannot open" << filename << endl;
		return 0;
	}

	// cut out the file name that want to send to client
	int len = strlen(filename);
	int i = len;
	while (filename[i] != '\\' && i >= 0) i--;
	if (i < 0) {
		i = 0;
	}
	else {
		i++;
	}
	int m = 0;
	while (filename[m + i] != '\0') {
		fname[m] = filename[m + i];
		m++;
	}

	// initialize winsock2.dll
	WSADATA wsaData;
	// generate version 2.2
	WORD wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		cout << "load winsock.dll failed" << endl;
		return 0;
	}

	// create socket
	if ((sock_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		cout << "create socket failed" << endl;
		WSACleanup();
		return 0;
	}

	// set socket to non-block pattern
	unsigned long u1 = 1;
	// set socket option parameter
	int nRet = ioctlsocket(sock_server, FIONBIO, (unsigned long*)&u1);
	if (nRet == SOCKET_ERROR) {
		cout << "set socket option failed" << endl;
		closesocket(sock_server);
		WSACleanup();
		return 0;
	}

	// binding ip and port
	memset((void*)&addr, 0, addr_len);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	// allow to use any ip of localhost
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if (bind(sock_server, (LPSOCKADDR)&addr, sizeof(addr) != 0)) {
		cout << "address bind failed. error code:" << WSAGetLastError() << endl;
		closesocket(sock_server);
		WSACleanup();
		return 0;
	}

	// set socket listenning
	if (listen(sock_server, 5) != 0) {
		cout << "listen function called failed. error code:" << WSAGetLastError() << endl;
		closesocket(sock_server);
		WSACleanup();
		return 0;
	}
	else {
		cout << "listenning..." << endl;
	}

	// start command receive thread
	hThread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetCommand, (void*)Command, 0, &ThreadId1);

	// receive and process the connect request from client
	SOCKET newsock;
	char client_ip[20] = { 0 };
	while (true) {
		// check command whether is "close" or not.
		// if the command is "close", then exit
		if (strcpy_s(Command, "close") == 0) {
			break;
		}
		else {
			newsock = accept(sock_server, (LPSOCKADDR)&client_addr, &addr_len);
			// after succeed in receiving a connection
			if (newsock != INVALID_SOCKET) {
				inet_pton(AF_INET, client_ip, (PVOID)&client_addr.sin_addr.S_un.S_addr);
				cout << "connect from:" << client_ip << endl;
				// start receive connect thread
				hThread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendFile, (void*)newsock, 0, &ThreadId2);
			}
			
		}
	}
		
	// release resource
	// close file
	inFile.close();
	// close connect socket
	closesocket(sock_server);
	// release winsock dll
	WSACleanup();
	cout << "hello world" << endl;
	//system("pause");
	//getchar();
	return 0;
}


unsigned int _stdcall SendFile(void *par) {
	char buffer[1000];
	SOCKET sock = (SOCKET)par;
	// set socket to block option
	unsigned long u1 = 0;
	// set socket option
	int nRet = ioctlsocket(sock, FIONBIO, (unsigned long*)&u1);
	// send file name
	send(sock, (char*)fname, strlen(fname) + 1, 0);
	// receive client send "OK" message
	int size = recv(sock, buffer, sizeof(buffer), 0);
	if (strcmp(buffer, "OK") != 0) {
		cout << "receive OK from client error" << endl;
		closesocket(sock);
		return 0;
	}

	// transfer file content
	while (!inFile.eof()) {
		inFile.read(fileBuffer, sizeof(fileBuffer));
		// get read size actually. 
		size = (int)inFile.gcount();
		send(sock, (char*)buffer, size, 0);
	}
	cout << "file transfer over" << endl;
	closesocket(sock);
	return 0;
}

unsigned int _stdcall GetCommand(void *par) {
	char *p = (char*)par;
	while (strcmp(p, "close") != 0) {
		gets_s(p, sizeof(p));
		//fgets(p, sizeof(p), stdin);
	}
	return 0;
}

//#endif

#if 0
int main(int argc, char *argv[])
{
	int t = 0;
	HANDLE htd0, htd1, htd2;
	DWORD thrdID0, thrdID2;
	htd0 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadF0, (void*)&t, 0, &thrdID0);
	htd1 = 0;
	//htd1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadF1, (void*)&t, 0, &thrdID1);
	htd2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadF2, (void*)&t, 0, &thrdID2);

	Sleep(1000);
	printf("t:%d\n", t);

	cout << "hello, world\n" << endl;
	return 0;
}

#endif