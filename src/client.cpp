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

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// Recebe mensagens indefinidamente em uma thread separada
int messageReader(SOCKET socket, thread &Thread)
{
	char server_reply[BUFFER_SIZE];
	int recv_size;

	while (true)
	{
		// Recebe a mensagem do servidor
		if ((recv_size = recv(socket, server_reply, BUFFER_SIZE, 0)) == SOCKET_ERROR)
		{
			cout << "Falha no retorno do servidor." << endl;
			break;
		}

		cout << server_reply << endl;
	}
	Thread.detach();
	return 0;
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "pt_BR.UTF-8");

	WSADATA wsa;
	char *ip = IP_ADDRESS;
	SOCKET client_socket;
	struct sockaddr_in server;
	string message, username;

	cout << "Inicializando Winsock..." << endl;
	
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Erro na inicialização. Código de Erro : %d", WSAGetLastError());
		return 1;
	}

	// Cria socket
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Erro na criação do socket : %d", WSAGetLastError());
	}

	cout << "Socket Criado." << endl;
	cout << "ip: " << ip << endl;

	// Prepara a estrutura sockaddr_in
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_port = htons(PORT);

	// Conecta ao servidor
	if (connect(client_socket, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		puts("Erro a conectar com o servidor");
		return 1;
	}

	cout << "Conectado" << endl;
	cout << "Insira o seu username: " << endl;
	getline(cin >> ws, username);
	cout << "Pressione '0' caso queira sair do chat" << endl;

	if (send(client_socket, username.c_str(), BUFFER_SIZE, 0) < 0)
	{
		cerr << "Envio Falhou" << endl;
		return 1;
	}

	thread thread_message_reader;
	thread_message_reader = thread(messageReader, client_socket, ref(thread_message_reader));

	while (TRUE)
	{
		// Recebe input como linha (para espaços)
		getline(cin >> ws, message);

		// Enviar zero para sair do chat
		if (message == "0")
		{
			cout << "Seção Encerrada." << endl;
			break;
		}

		if (send(client_socket, message.c_str(), BUFFER_SIZE, 0) < 0)
		{
			cerr << "Falha no envio" << endl;
			return 1;
		}
	}

	closesocket(client_socket);
	system("pause");

	return 0;
}