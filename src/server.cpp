#include <iostream>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <locale.h>
#include <thread>
#include <vector>
#include <map>

#pragma comment(lib, "Ws2_32.lib")

#define IP_ADDRESS "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 150

using namespace std;

class Client
{
public:
    int id;
    SOCKET socket;
    string name;
    Client(int id, SOCKET socket, string name): id(id), socket(socket), name(name) {};
    ~Client();
};


const char OPTION_VALUE = 1;
const int MAX_CLIENTS = 5;


mutex client_array_mutex;
Client *client_array[MAX_CLIENTS];
thread clients_thread[MAX_CLIENTS];
map<SOCKET, thread> handle_clients;

void broadcastMessage(Client *client, string message)
{


    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_array[i]->socket != INVALID_SOCKET)
            if (client->id != i)
                send(client_array[i]->socket, message.c_str(), BUFFER_SIZE, 0);
    }
}

int process_client(Client *client)
{
    string message = "";
    char buffer[BUFFER_SIZE] = {0};
    cout << "BBB1" << endl;
    //Session
    while (1)
    {
        // Limpa a mensagem temporária substituindo por zeros
        // cout << "bb1" << client->socket << endl;

        if (client->socket != INVALID_SOCKET)
        {
            int data_received = recv(client->socket, buffer, BUFFER_SIZE, 0);

            if (data_received != SOCKET_ERROR)
            {

                message = client->name + ": " + string(buffer);

                cout << message.c_str() << endl;
                // trava
                //Broadcast that message to the other clients
                broadcastMessage(client, message);
            }
            else
            {
                message = client->name + " Disconnected";

                cout << message << endl;
                client_array_mutex.lock();
                closesocket(client->socket);
                closesocket(client_array[client->id]->socket);
                client_array[client->id]->socket = INVALID_SOCKET;
                client_array_mutex.unlock();
                //Broadcast the disconnection message to the other clients
                broadcastMessage(client, message);

                break;
            }
        }
    } //end while

    (clients_thread[client->id]).detach();

    return 0;
}

int handle_client(SOCKET client_socket)
{
    char buffer[BUFFER_SIZE] = {0};
    bool is_inserted = false;
    Client *new_client;
    string message;

    if (recv(client_socket, buffer, BUFFER_SIZE, 0) < 0)
    {
        cerr << "Erro ao iniciar o usuario." << endl;
    }

    string username = buffer;

    client_array_mutex.lock();

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_array[i]->socket == INVALID_SOCKET)
        {
            client_array[i]->socket = client_socket;
            // printf("Adding to list of sockets at index %d \n", i);
            
            client_array[i]->id = i;
            cout << "alo 1" << endl;
            client_array[i]->name = username;
            // username.cop
            cout << "alo" << endl;
            
            is_inserted = true;
            new_client = client_array[i];
            break;
        }
    } // new, malloc

    client_array_mutex.unlock();
    if (is_inserted)
    {
        cout << username << " Entrou na sala." << endl;
        message = username + " Entrou na sala.";
        broadcastMessage(new_client, message);
        cout << "b4" << endl;
        clients_thread[new_client->id] = thread(process_client, new_client);
        cout << "b5" << endl;
    }
    else
    {
        cout << "b6" << endl;
        message = "Servidor está cheio.";
        send(client_socket, message.c_str(), BUFFER_SIZE, 0);
        cout << "b7" << endl;
        cout << message << endl;
    }
    (handle_clients[client_socket]).detach();
    return 0;
}

int main()
{
    setlocale(LC_ALL, "Portuguese");

    WSADATA wsaData;
    struct sockaddr_in server, address;
    SOCKET master;
    string message = "";
    int num_clients = 0, addrlen;

    char buffer[BUFFER_SIZE] = {0};
    // buffer =  (char*) malloc((BUFFER_SIZE + 1) * sizeof(char));

    //Initialize Winsock
    cout << "Inicializando o Winsock..." << endl;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //Create a socket
    if ((master = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    cout << "Criando Socket..." << endl;

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    //Bind
    if (bind(master, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    cout << "Conectado a porta " << PORT << "..." << endl;

    //Listen to new_socket connections
    listen(master, MAX_CLIENTS);

    cout << "Aguardando Usuarios..." << endl;

    //Initialize the client list
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        // Client temp = 
        client_array[i] =  new Client(-1, INVALID_SOCKET, "");
    }

    addrlen = sizeof(struct sockaddr_in);

    string username;
    while (1)
    {
        bool is_inserted = false;
        SOCKET new_socket = INVALID_SOCKET;
        new_socket = accept(master, (struct sockaddr *)&address, (int *)&addrlen);
        if (new_socket != INVALID_SOCKET)
        {
            cout << "antes da thread " << new_socket << endl;
            handle_clients[new_socket] = thread(handle_client, new_socket);
        }

    } //end while

    //Close listening socket
    closesocket(master);

    //Close client socket
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients_thread[i].detach();
        closesocket(client_array[i]->socket);
    }

    system("pause");
    //Clean up Winsock
    WSACleanup();
    cout << "Program has ended successfully" << endl;
}