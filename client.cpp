// client.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)
#include <Winsock2.h>
#include <iostream>
#include <ctime>
using namespace std;

SOCKET connection;

int main() {
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		cout << "Error in dll" << endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1500);
	addr.sin_family = AF_INET;

	connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		cout << "Connection failed!\r\n";
		return 1;
	}
	cout << "Connection successful!\r\n";
	cout << "You can write \"help\" if you want to take a list of commands.\r\n";
	char msg[1024] = { };
	recv(connection, msg, sizeof(msg), NULL);
	cout << msg;
	while (true) {
		char msg[1024] = { };
		cin >> msg;
		send(connection, msg, sizeof(msg), NULL);
		Sleep(200);
		recv(connection, msg, sizeof(msg), NULL);
		cout << msg;
	}

	system("pause");
	return 0;
}