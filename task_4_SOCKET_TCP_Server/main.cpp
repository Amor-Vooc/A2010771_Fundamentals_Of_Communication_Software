#include <ws2tcpip.h>
#include <winsock2.h>
#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <algorithm>
#include <csignal>
#include <map>
#include <string>
//2023211281-��ͬ��-SOCKET_TCP_Server

using namespace std;

#pragma comment(lib, "ws2_32.lib")

class ChatServer {
public:
    ChatServer(const string& IP, int port);
    void Start();
    void Stop();
    static void SignalHandler(int signal);
    static ChatServer* serverInstance;

private:
    void HandleClient(SOCKET clientSocket);
    void BroadcastMessage(const char* message, SOCKET senderSocket, int type);
    void PrintError(const string& message);

    vector<SOCKET> clients; // �洢�ͻ����׽���
    map<SOCKET, string> clientUsernames; // �洢�ͻ����û���
    mutex clientsMutex; // �����Կͻ��˵ķ���
    SOCKET serverSocket;
    bool running = false;
    int accessCount = 0;
};

ChatServer* ChatServer::serverInstance = nullptr;

ChatServer::ChatServer(const string& IP, int port) {
    WSADATA wd;
    if (WSAStartup(MAKEWORD(2, 2), &wd) != 0) {
        PrintError("WSAStartupʧ��");
        exit(1);
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        PrintError("�����׽���ʧ��");
        exit(1);
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, IP.c_str(), &addr.sin_addr);

    if (bind(serverSocket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        PrintError("��ʧ��");
        exit(1);
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        PrintError("����ʧ��");
        exit(1);
    }

    cout << "���������ڼ����˿� " << port << "..." << endl;
    running = true;
}

void ChatServer::Start() {
    while (running) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket != INVALID_SOCKET) {
            {
                lock_guard<mutex> lock(clientsMutex);
                clients.push_back(clientSocket);
                accessCount++; // ���ӷ��ʼ���
                cout << "��ǰ���ʴ���: " << accessCount << endl;
            }
            thread(&ChatServer::HandleClient, this, clientSocket).detach();
        }
    }
}

void ChatServer::HandleClient(SOCKET clientSocket) {
    //cout << "��ӭ " << clientSocket << " ���������ң�" << endl;
    char username[100] = { 0 };

    // �����û���
    recv(clientSocket, username, sizeof(username), 0);
    {
        lock_guard<mutex> lock(clientsMutex);
        clientUsernames[clientSocket] = username; // �洢�û���
    }

    cout << "��ӭ " << username << " ���������ң�" << endl;


    char welcomeMessage[100];
    //snprintf(welcomeMessage, sizeof(welcomeMessage), "��ӭ %d ����������!", clientSocket);
    snprintf(welcomeMessage, sizeof(welcomeMessage), "��ӭ %s ����������!", username);
    send(clientSocket, welcomeMessage, sizeof(welcomeMessage), 0);
    BroadcastMessage(welcomeMessage, clientSocket, 0);
    

    char buf[100] = { 0 };
    int ret;

    do {
        memset(buf, 0, sizeof(buf));
        ret = recv(clientSocket, buf, sizeof(buf), 0);

        if (ret > 0) {
            cout << username << " ˵��" << buf << endl;
            BroadcastMessage(buf, clientSocket, 1);
        }
    } while (ret > 0);

    {
        lock_guard<mutex> lock(clientsMutex);
        clients.erase(remove(clients.begin(), clients.end(), clientSocket), clients.end());
        clientUsernames.erase(clientSocket); // �Ƴ��û���
    }

    cout << username << " �뿪��������" << endl;
    closesocket(clientSocket);
}

void ChatServer::BroadcastMessage(const char* message, SOCKET senderSocket, int type) {
    lock_guard<mutex> lock(clientsMutex);
    for (const SOCKET& otherClient : clients) {
        if (otherClient != senderSocket) {
            char msg[100];
            if (type) {
                snprintf(msg, sizeof(msg), "%s ˵��%s", clientUsernames[senderSocket].c_str(), message);
            }
            else {
                snprintf(msg, sizeof(msg), "�³�Ա���룡%s", message);
            }
            send(otherClient, msg, sizeof(msg), 0);
        }
    }
}

void ChatServer::PrintError(const string& message) {
    cout << message << ": " << GetLastError() << endl;
}

void ChatServer::Stop() {
    running = false; // ���ñ�־Ϊfalse���˳���ѭ��
    closesocket(serverSocket); // �رռ����׽���
    WSACleanup(); // ����Winsock
    cout << "�������ѹرա�" << endl;
}

// �źŴ�����
void ChatServer::SignalHandler(int signal) {
    if (serverInstance) {
        serverInstance->Stop();
    }
}

int main(int argc, char* argv[]) {

    string IP = "127.0.0.1";
    int PORT = 3000;

    if (argc ==1) {
        cout << "δ����ָ�� IP �Ͷ˿ںţ�Ĭ��ʹ�� 127.0.0.1:3000" << endl;
    }
    else if (argc == 2) {
        cout << "IP : " << argv[1] << "��δ����ָ���˿ںţ�Ĭ��ʹ�� 3000" << endl;
        IP = argv[1];
    }
    else if (argc == 3) {
        cout << "�������ӣ�" << argv[1] << ":" << argv[2] << endl;
        IP = argv[1];
        PORT = std::stoi(argv[2]); // ����char* ת int
    }

    ChatServer server(IP, PORT);
    ChatServer::serverInstance = &server;

    // �����źŴ���
    signal(SIGINT, ChatServer::SignalHandler);
    signal(SIGTERM, ChatServer::SignalHandler);

    server.Start();

    return 0;
}
