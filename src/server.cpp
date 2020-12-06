#include <iostream>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <locale.h>
#include <thread>
#include <vector>
#include <map>
#include <chrono>
#include <ctime>

#pragma comment(lib, "Ws2_32.lib")

#define IP_ADDRESS "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 150

using namespace std;

class Client
{
public:
    // id se comporta como incrementador
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



// Envia mensagem para todos os clientes conectados, exceto o que enviou
// *client -> Ponteiro para o cliente que enviou
void broadcastMessage(Client *client, string message)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_array[i]->socket != INVALID_SOCKET)
            if (client->id != i)
                send(client_array[i]->socket, message.c_str(), BUFFER_SIZE, 0);
    }
}


string getTime(){
    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());

    string s(30, '\0');
    strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return s;
}

int client_session(Client *client)
{
    string message = "";
    char buffer[BUFFER_SIZE] = {0};
    
    while (1)
    {
        // Recebe mensagem, printa e envia para demais clientes
        if (client->socket != INVALID_SOCKET)
        {
            int data_received = recv(client->socket, buffer, BUFFER_SIZE, 0);

            if (data_received != SOCKET_ERROR)
            {

                message = client->name + ": " + string(buffer);

                cout << getTime() << endl;
                cout << message << endl;

                broadcastMessage(client, message);
            }
            // Caso receba socket error, remove sockets e o cliente do array
            else
            {   
                message = client->name + " desconectado";
                cout << message << endl;

                // região crítica           
                client_array_mutex.lock();
                
                closesocket(client->socket);
                closesocket(client_array[client->id]->socket);
                client_array[client->id]->socket = INVALID_SOCKET;
                
                client_array_mutex.unlock();

                //Envia a mensagem de deconexão para demais clientes
                broadcastMessage(client, message);

                break;
            }
        }
    }
    (clients_thread[client->id]).detach();

    return 0;
}

// Lida com o client no momento da conexão                
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
    // Procura primeiro espaço no array de clientes e insere o novo cliente
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_array[i]->socket == INVALID_SOCKET)
        {
            client_array[i]->socket = client_socket;
            client_array[i]->id = i;
            client_array[i]->name = username;
            
            is_inserted = true;
            
            printf("Cliente adicionado, id: %d \n", i);
            // new_cliente criado para uso posterior dos valores do cliente
            new_client = client_array[i];
            break;
        }
    }
    client_array_mutex.unlock();
    
    if (is_inserted)
    {
        message = username + " se juntou à sala.";
        cout << message << endl;
        broadcastMessage(new_client, message);
        // Cria thread para o cliente
        clients_thread[new_client->id] = thread(client_session, new_client);
    }
    // Único caso para não ser inserido, é o servidor estar cheio
    else
    {
        message = "O servidor está cheio.";
        // Retorna apenas para o cliente que tentou entrar
        send(client_socket, message.c_str(), BUFFER_SIZE, 0);
        cout << message << endl;
    }
    (handle_clients[client_socket]).detach();
    return 0;
}

int main()
{
    setlocale(LC_ALL, "pt_BR.UTF-8");

    WSADATA wsaData;
    struct sockaddr_in server, address;
    SOCKET master;
    string message = "", username;
    int num_clients = 0, addrlen;

    char buffer[BUFFER_SIZE] = {0};
    // buffer =  (char*) malloc((BUFFER_SIZE + 1) * sizeof(char));


    cout << "Inicializando o Winsock..." << endl;
    WSAStartup(MAKEWORD(2, 2), &wsaData);


    // Cria socket principal
    if ((master = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Socket não pode ser criado: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    // Prepara a estrutura sockaddr_in
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    cout << "Conectando à porta " << PORT << "..." << endl;
    // Conecta o socket à porta
    if (bind(master, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind falhou: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }


    // Passa a aguardar novos sockets
    listen(master, MAX_CLIENTS);

    cout << "Aguardando Usuarios..." << endl;

    // Inicializa lista de clientes
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        // Client temp = 
        client_array[i] =  new Client(-1, INVALID_SOCKET, "");
    }

    addrlen = sizeof(struct sockaddr_in);

    // Loop principal, recebe sockets e cria uma thread nova para lidar com eles
    while (1)
    {
        bool is_inserted = false;
        SOCKET new_socket = INVALID_SOCKET;

        new_socket = accept(master, (struct sockaddr *)&address, (int *)&addrlen);
        
        if (new_socket != INVALID_SOCKET)
        {
            handle_clients[new_socket] = thread(handle_client, new_socket);
        }

    }

    closesocket(master);

    // Finaliza todos os sockets e threads dos clientes 
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients_thread[i].detach();
        closesocket(client_array[i]->socket);
    }
    
    WSACleanup();
    
    cout << "Servidor finalizado." << endl;
    
    system("pause");
}