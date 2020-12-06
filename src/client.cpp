/*
	Get IP address from domain name
*/
#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include <cstdio>
#include <string>
#include <thread>

#define IP_ADDRESS "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 150

#pragma comment(lib, "ws2_32.lib") //Winsock Library

using namespace std;

int messageReader(SOCKET socket, thread &Thread)
{
	char server_reply[BUFFER_SIZE];
	int recv_size;

	while (true)
	{
		//Receive a reply from the server
		if ((recv_size = recv(socket, server_reply, BUFFER_SIZE, 0)) == SOCKET_ERROR)
		{
			puts("recv failed");
			break;
		}

		cout << server_reply << endl;
	}
	Thread.detach();
	return 0;
}

int main(int argc, char *argv[])
{
	WSADATA wsa;
	char *ip = IP_ADDRESS;
	SOCKET client_socket;
	struct sockaddr_in server;

	cout << "Inicializando Winsock" << endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Erro na inicialização. Código de Erro : %d", WSAGetLastError());
		return 1;
	}

	cout << "Incializado." << endl;

	//Create a socket
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Erro na criação do socket : %d", WSAGetLastError());
	}

	cout << "Socket Criado." << endl;

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;

	cout << "ip: " << ip << endl;
	server.sin_addr.s_addr = inet_addr(ip);

	server.sin_port = htons(PORT);

	//Connect to remote server
	if (connect(client_socket, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}

	cout << "Conectado" << endl;

	// //Bind
	// if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
	// {
	// 	printf("Bind failed with error code : %d" , WSAGetLastError());
	// }

	// puts("Bind done");
	string message;

	cout << "Insira o seu username: " << endl;
	cin >> message;

	if (send(client_socket, message.c_str(), BUFFER_SIZE, 0) < 0)
	{
		cerr << "Envio Falhou" << endl;
		return 1;
	}

	thread thread_message_reader;
	thread_message_reader = thread(messageReader, client_socket, ref(thread_message_reader));

	while (TRUE)
	{
		//Send some data

		getline(cin >> ws, message);
		if (message == "0")
		{
			cout << "Seção Encerrada." << endl;
			break;
		}

		if (send(client_socket, message.c_str(), BUFFER_SIZE, 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
		puts("Data Send\n");
	}

	system("pause");
	closesocket(client_socket);

	return 0;
	return 0;
}