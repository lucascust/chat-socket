/*
	Get IP address from domain name
*/
#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include <cstdio>
#include <string>

#define IP_ADDRESS "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 150

#pragma comment(lib,"ws2_32.lib") //Winsock Library

using namespace std;

int main(int argc , char *argv[])
{
	WSADATA wsa;
	char *ip = IP_ADDRESS;
	SOCKET s;
	struct sockaddr_in server;
	
	cout << "Inicializando Winsock" << endl;
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Erro na inicialização. Código de Erro : %d",WSAGetLastError());
		return 1;
	}
	
	cout << "Incializado." << endl;
	

	//Create a socket
	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Erro na criação do socket : %d" , WSAGetLastError());
	}

	cout << "Socket Criado." << endl;

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	
	cout << "ip: " << ip << endl; 
	server.sin_addr.s_addr = inet_addr(ip);

	server.sin_port = htons(PORT);
	
	//Connect to remote server
	if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
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

	while (TRUE){
		//Send some data
		cin >> message;



		cout << message << endl;

		// if(!(char_traits<char>::compare(message, "0", 1))){
		// 	cout << "Seção Encerrada." << endl;
		// 	break;
		// }


		if( send(s , message.c_str() , BUFFER_SIZE , 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
		puts("Data Send\n");
		
		
		char server_reply[BUFFER_SIZE];
		int recv_size;
		//Receive a reply from the server
		if((recv_size = recv(s , server_reply , BUFFER_SIZE , 0)) == SOCKET_ERROR)
		{
			puts("recv failed");
		}
		
		cout << server_reply << endl;


	}
	
	
	
	system("pause");
	closesocket(s);
	
	return 0;
	return 0;
}