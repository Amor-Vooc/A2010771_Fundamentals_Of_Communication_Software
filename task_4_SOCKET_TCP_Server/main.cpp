#include <ws2tcpip.h>
#include <winsock2.h>
#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <algorithm>
#include <csignal>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

class ChatServer {
public:
    ChatServer(int port);
    void Start();
    void Stop();
    static void SignalHandler(int signal);
    static ChatServer* serverInstance;

private:
    void HandleClient(SOCKET clientSocket);
    void BroadcastMessage(const char* message, SOCKET senderSocket);
    void PrintError(const string& message);

    vector<SOCKET> clients; // �洢�ͻ����׽���
    mutex clientsMutex; // �����Կͻ��˵ķ���
    SOCKET serverSocket;
    bool running = false;
    int accessCount = 0;
};

ChatServer* ChatServer::serverInstance = nullptr;

ChatServer::ChatServer(int port) {
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
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

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
    cout << "��ӭ " << clientSocket << " ���������ң�" << endl;

    char welcomeMessage[100];
    snprintf(welcomeMessage, sizeof(welcomeMessage), "��ӭ %d ����������!", clientSocket);
    send(clientSocket, welcomeMessage, sizeof(welcomeMessage), 0);

    char buf[100] = { 0 };
    int ret;

    do {
        memset(buf, 0, sizeof(buf));
        ret = recv(clientSocket, buf, sizeof(buf), 0);

        if (ret > 0) {
            cout << clientSocket << " ˵: " << buf << endl;
            BroadcastMessage(buf, clientSocket);
        }
    } while (ret > 0);

    {
        lock_guard<mutex> lock(clientsMutex);
        clients.erase(remove(clients.begin(), clients.end(), clientSocket), clients.end());
    }

    cout << "�û� " << clientSocket << " �뿪��������" << endl;
    closesocket(clientSocket);
}

void ChatServer::BroadcastMessage(const char* message, SOCKET senderSocket) {
    lock_guard<mutex> lock(clientsMutex);
    for (const SOCKET& otherClient : clients) {
        if (otherClient != senderSocket) {
            char msg[100];
            snprintf(msg, sizeof(msg), "%d ˵��%s", senderSocket, message);
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

int main() {
    ChatServer server(1145);
    ChatServer::serverInstance = &server;

    // �����źŴ���
    signal(SIGINT, ChatServer::SignalHandler);
    signal(SIGTERM, ChatServer::SignalHandler);

    server.Start();

    return 0;
}
