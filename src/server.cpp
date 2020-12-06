#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <locale.h>  
#include <thread>
#include <vector>
 
#pragma comment (lib, "Ws2_32.lib")
 
#define IP_ADDRESS "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 150
 
struct client_t
{
    int id;
    char name[30];
    SOCKET socket;
};
 
const char OPTION_VALUE = 1;
const int MAX_CLIENTS = 5;
 
using namespace std;

void broadcastMessage(vector<client_t> &client_array, client_t &client, string message){
    
    int msg_len = message.length();
    char* char_msg = new char[msg_len + 1];
    strcpy(char_msg, message.c_str());

    for (int i = 0; i < MAX_CLIENTS; i++)
        {
        if (client_array[i].socket != INVALID_SOCKET)
            if (client.id != i)
                send(client_array[i].socket, char_msg, msg_len, 0);
        }
}

 
int process_client(client_t &client, vector<client_t> &client_array, thread &thread)
{
    string message = "";
    char temporary_message[BUFFER_SIZE] = "";
 
    //Session
    while (1)
    {   
        // Limpa a mensagem temporária substituindo por zeros
        memset(temporary_message, 0, BUFFER_SIZE);
 
        if (client.socket != 0)
        {
            int data_received = recv(client.socket, temporary_message, BUFFER_SIZE, 0);
 
            if (data_received != SOCKET_ERROR)
            {
                
                message = string(client.name) + ": " + string(temporary_message);
 
                cout << message.c_str() << endl;
 
                //Broadcast that message to the other clients
                broadcastMessage(client_array, client, message);
            }
            else
            {
                message = string(client.name) + "Disconnected";
 
                cout << message << endl;
 
                closesocket(client.socket);
                closesocket(client_array[client.id].socket);
                client_array[client.id].socket = INVALID_SOCKET;
 
                //Broadcast the disconnection message to the other clients
                broadcastMessage(client_array, client, message);
 
                break;
            }
        }
    } //end while
 
    thread.detach();
 
    return 0;
}
 
int main()
{
     setlocale(LC_ALL, "Portuguese");

    WSADATA wsaData;
    struct sockaddr_in server, address;
    SOCKET master, new_socket;
    string message = "";
    int num_clients = 0, addrlen;
    char username[30];

    char *buffer;
	buffer =  (char*) malloc((BUFFER_SIZE + 1) * sizeof(char));
    
    vector<client_t> client_array(MAX_CLIENTS);
    client_t new_client;
    thread my_thread[MAX_CLIENTS];
 
    //Initialize Winsock
    cout << "Inicializando o Winsock..." << endl;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
 
    //Create a socket
	if((master = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d" , WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	cout << "Criando Socket..." << endl;
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( PORT );
	
	//Bind
	if( bind(master ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d" , WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	
	cout << "Conectando a porta " << PORT << "..." << endl;

	//Listen to new_socket connections
	listen(master , MAX_CLIENTS);
 
    //Initialize the client list
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_array[i] = { -1, "", INVALID_SOCKET };
    }

    addrlen = sizeof(struct sockaddr_in);

    while (1)
    {
        bool is_inserted = false;
        SOCKET new_socket = INVALID_SOCKET;
        new_socket = accept(master, (struct sockaddr *)&address, (int *)&addrlen);
 
        int data_rec = recv(new_socket, buffer, BUFFER_SIZE, 0);

        //buffer[data_rec] = '\0';
        
        cout << buffer << endl;
        cout << sizeof(buffer) << endl;

        cout << username << endl;
        cout << sizeof(username) << endl;
        
        
        cout << "ainda nao deu ruim" << endl;
        strcpy_s(username, strlen(buffer) - 1, buffer);
        cout << "ainda nao deu ruim" << endl;

        if (new_socket == INVALID_SOCKET) continue;
 
        //Reset the number of clients
        num_clients = -1;


 
        for (int i = 0; i < MAX_CLIENTS; i++) 
        {
            if (client_array[i].socket == INVALID_SOCKET)
            {
                client_array[i].socket = new_socket;
                printf("Adding to list of sockets at index %d \n" , i);

                client_array[i].id = i;
                strncpy(client_array[i].name, username, sizeof(username));

                new_client = client_array[i];
                
                is_inserted = true;
                break;
            }
        }
 
        if (is_inserted)
        {
            cout << username << " Accepted" << endl;
            message = to_string(new_client.id);
            send(new_client.socket, message.c_str(), strlen(message.c_str()), 0);
 
            //Create a thread process for that client
            my_thread[new_client.id] = thread(process_client, ref(new_client), ref(client_array), ref(my_thread[new_client.id]));
        }
        else
        {
            message = "Server is full";
            send(new_socket, message.c_str(), strlen(message.c_str()), 0);
            cout << message << endl;
        }
    } //end while
 
 
    //Close listening socket
    closesocket(master);
 
    //Close client socket
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        my_thread[i].detach();
        closesocket(client_array[i].socket);
    }
 
    //Clean up Winsock
    WSACleanup();
    cout << "Program has ended successfully" << endl;
 
    system("pause");
    return 0;
}