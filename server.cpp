// server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)
#include <Winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <math.h>
#include <string>

using namespace std;

#define SERVICE_PORT 1500 

int send_string(SOCKET s, const char* sString);

int main(void) {
	SOCKET  S;  //дескриптор прослушивающего сокета
	SOCKET  NS; //дескриптор присоединенного сокета

	sockaddr_in serv_addr;
	WSADATA     wsadata;
	char        sName[128];
	bool        bTerminate = false;

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		cout << "Error in dll" << endl;
		exit(1);
	}

	gethostname(sName, sizeof(sName));
	printf("\nServer host: %s\n", sName);

	if ((S = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		fprintf(stderr, "Can't create socket\n");
		exit(1);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons((u_short)SERVICE_PORT);
	if (bind(S, (sockaddr*)&serv_addr, sizeof(serv_addr)) == INVALID_SOCKET) {
		fprintf(stderr, "Can't bind\n");
		exit(1);
	}
	if (listen(S, 5) == INVALID_SOCKET) {
		fprintf(stderr, "Can't listen\n");
		exit(1);
	}

	printf("Server listen on %s:%d\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));

	while (!bTerminate) {
		printf("Wait for connections.....\n");

		sockaddr_in clnt_addr;
		int addrlen = sizeof(clnt_addr);
		memset(&clnt_addr, 0, sizeof(clnt_addr));

		NS = accept(S, (sockaddr*)&clnt_addr, &addrlen);
		if (NS == INVALID_SOCKET) {
			fprintf(stderr, "Can't accept connection\n");
			break;
		}
		addrlen = sizeof(serv_addr);
		getsockname(NS, (sockaddr*)&serv_addr, &addrlen);
		printf("Accepted connection on %s:%d ", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
		printf("from client %s:%d\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
		send_string(NS, "* * * Welcome to simple UPCASE TCP-server * * *\r\n");
		char    sReceiveBuffer[1024] = { 0 };
		// Получаем и обрабатываем данные от клиента
		while (true) {
			int nReaded = recv(NS, sReceiveBuffer, sizeof(sReceiveBuffer) - 1, 0);
			// В случае ошибки (например, отсоединения клиента) выходим
			if (nReaded <= 0) break;
			// Мы получаем поток байт, поэтому нужно самостоятельно 
			// добавить завержающий 0 для ASCII строки 
			sReceiveBuffer[nReaded] = 0;

			// Отбрасываем символы превода строк
			for (char* pPtr = sReceiveBuffer; *pPtr != 0; pPtr++) {
				if (*pPtr == '\n' || *pPtr == '\r') {
					*pPtr = 0;
					break;
				}
			}
			// Пропускаем пустые строки
			if (sReceiveBuffer[0] == 0) continue;

			printf("Received data: %s\n", sReceiveBuffer);

			// Анализируем полученные команды или преобразуем текст в верхний регистр
			if (strcmp(sReceiveBuffer, "info") == 0) {
				send_string(NS, "Griminov Alexander Valerivich\r\n");
			}
			else if (strcmp(sReceiveBuffer, "help") == 0) {
				send_string(NS, "\tinfo\n\ttime\n\tstrlen\n\ttask\n\texit\n\tshutdown\n");
			}
			else if (strcmp(sReceiveBuffer, "time") == 0) {
				time_t now = time(0);
				tm* localtm = localtime(&now);
				send_string(NS, "The local date and time is: ");
				send_string(NS, asctime(localtm));
				send_string(NS, "\r");
			}
			else if (strcmp(sReceiveBuffer, "strlen") == 0) {
				char cl[100];
				send_string(NS, "Enter the string:\r\n");
				while (recv(NS, cl, sizeof(cl), NULL) < 4) {
					Sleep(100);
				};
				int lo = strlen(cl);
				itoa(lo,cl,10);
				send_string(NS, "Lenght ");
				send_string(NS, cl);
				send_string(NS, "\r\n");
			}
			else if (strcmp(sReceiveBuffer, "task") == 0) {
				send_string(NS, "Variant 4\r\n");
				send_string(NS, "Add to the service which you can find out the length of the string.\r\nInput parameter : visible line.\r\nServer response : line length.\r\n");
			}
			else if (strcmp(sReceiveBuffer, "exit") == 0) {
				send_string(NS, "Bye...\r\n");
				printf("Client initialize disconnection.\r\n");
				break;
			}
			else if (strcmp(sReceiveBuffer, "shutdown") == 0) {
				send_string(NS, "Server go to shutdown.\r\n");
				Sleep(200);
				bTerminate = true;
				break;
			}
			else {
				char sSendBuffer[1024];
				// Преобразовываем строку в верхний регистр
				_snprintf(sSendBuffer, sizeof(sSendBuffer), "Server reply: %s\r\n",
					_strupr(sReceiveBuffer));
				send_string(NS, sSendBuffer);
			}
		}
		// закрываем присоединенный сокет
		closesocket(NS);
		printf("Client disconnected.\n");
	}
	// Закрываем серверный сокет
	closesocket(S);
	// освобождаем ресурсы библиотеки сокетов
	WSACleanup();
	return 0;
}

// Функция отсылки текстовой ascii строки клиенту
int send_string(SOCKET s, const char* sString) {
	return send(s, sString, strlen(sString), 0);
}