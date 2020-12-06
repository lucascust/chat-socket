/*
	Get IP address from domain name
*/
#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include <cstdio>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib") //Winsock Library

using namespace std;

int main(int argc , char *argv[])
{
	WSADATA wsa;
	char *hostname = "www.google.com";
	char ip[100];
	struct hostent *he;
	struct in_addr **addr_list;
	int i;
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


	// Faz ip
	if ( (he = gethostbyname( hostname ) ) == NULL) 
	{
		//gethostbyname failed
		printf("Falha ao receber ip : %d" , WSAGetLastError());
		return 1;
	}
	
	//Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
	addr_list = (struct in_addr **) he->h_addr_list;
	
	for(i = 0; addr_list[i] != NULL; i++) 
	{
		//Return the first one;
		strcpy_s(ip , inet_ntoa(*addr_list[i]) );

	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	
	cout << "ip: " << ip << endl; 
	server.sin_addr.s_addr = inet_addr(ip);

	server.sin_port = htons(80);
	
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
	
	char *message;
	//Send some data
	message = "GET / HTTP/1.1\r\n\r\n";
	if( send(s , message , strlen(message) , 0) < 0)
	{
		puts("Send failed");
		return 1;
	}
	puts("Data Send\n");
	
	char server_reply[2000];
	int recv_size;
	//Receive a reply from the server
	if((recv_size = recv(s , server_reply , 2000 , 0)) == SOCKET_ERROR)
	{
		puts("recv failed");
	}
	
	puts("Reply received\n");
	puts(server_reply);
	
	
	
	closesocket(s);

	printf("%s resolved to : %s\n" , hostname , ip);
	
	system("pause");
	
	return 0;
	return 0;
}