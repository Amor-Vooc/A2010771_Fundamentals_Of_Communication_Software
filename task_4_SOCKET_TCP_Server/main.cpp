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
//2023211281-丁同勖-SOCKET_TCP_Server

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

    vector<SOCKET> clients; // 存储客户端套接字
    map<SOCKET, string> clientUsernames; // 存储客户端用户名
    mutex clientsMutex; // 保护对客户端的访问
    SOCKET serverSocket;
    bool running = false;
    int accessCount = 0;
};

ChatServer* ChatServer::serverInstance = nullptr;

ChatServer::ChatServer(const string& IP, int port) {
    WSADATA wd;
    if (WSAStartup(MAKEWORD(2, 2), &wd) != 0) {
        PrintError("WSAStartup失败");
        exit(1);
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        PrintError("创建套接字失败");
        exit(1);
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, IP.c_str(), &addr.sin_addr);

    if (bind(serverSocket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        PrintError("绑定失败");
        exit(1);
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        PrintError("监听失败");
        exit(1);
    }

    cout << "服务器正在监听端口 " << port << "..." << endl;
    running = true;
}

void ChatServer::Start() {
    while (running) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket != INVALID_SOCKET) {
            {
                lock_guard<mutex> lock(clientsMutex);
                clients.push_back(clientSocket);
                accessCount++; // 增加访问计数
                cout << "当前访问次数: " << accessCount << endl;
            }
            thread(&ChatServer::HandleClient, this, clientSocket).detach();
        }
    }
}

void ChatServer::HandleClient(SOCKET clientSocket) {
    //cout << "欢迎 " << clientSocket << " 进入聊天室！" << endl;
    char username[100] = { 0 };

    // 接收用户名
    recv(clientSocket, username, sizeof(username), 0);
    {
        lock_guard<mutex> lock(clientsMutex);
        clientUsernames[clientSocket] = username; // 存储用户名
    }

    cout << "欢迎 " << username << " 进入聊天室！" << endl;


    char welcomeMessage[100];
    //snprintf(welcomeMessage, sizeof(welcomeMessage), "欢迎 %d 进入聊天室!", clientSocket);
    snprintf(welcomeMessage, sizeof(welcomeMessage), "欢迎 %s 进入聊天室!", username);
    send(clientSocket, welcomeMessage, sizeof(welcomeMessage), 0);
    BroadcastMessage(welcomeMessage, clientSocket, 0);
    

    char buf[100] = { 0 };
    int ret;

    do {
        memset(buf, 0, sizeof(buf));
        ret = recv(clientSocket, buf, sizeof(buf), 0);

        if (ret > 0) {
            cout << username << " 说：" << buf << endl;
            BroadcastMessage(buf, clientSocket, 1);
        }
    } while (ret > 0);

    {
        lock_guard<mutex> lock(clientsMutex);
        clients.erase(remove(clients.begin(), clients.end(), clientSocket), clients.end());
        clientUsernames.erase(clientSocket); // 移除用户名
    }

    cout << username << " 离开了聊天室" << endl;
    closesocket(clientSocket);
}

void ChatServer::BroadcastMessage(const char* message, SOCKET senderSocket, int type) {
    lock_guard<mutex> lock(clientsMutex);
    for (const SOCKET& otherClient : clients) {
        if (otherClient != senderSocket) {
            char msg[100];
            if (type) {
                snprintf(msg, sizeof(msg), "%s 说：%s", clientUsernames[senderSocket].c_str(), message);
            }
            else {
                snprintf(msg, sizeof(msg), "新成员加入！%s", message);
            }
            send(otherClient, msg, sizeof(msg), 0);
        }
    }
}

void ChatServer::PrintError(const string& message) {
    cout << message << ": " << GetLastError() << endl;
}

void ChatServer::Stop() {
    running = false; // 设置标志为false，退出主循环
    closesocket(serverSocket); // 关闭监听套接字
    WSACleanup(); // 清理Winsock
    cout << "服务器已关闭。" << endl;
}

// 信号处理函数
void ChatServer::SignalHandler(int signal) {
    if (serverInstance) {
        serverInstance->Stop();
    }
}

int main(int argc, char* argv[]) {

    string IP = "127.0.0.1";
    int PORT = 3000;

    if (argc ==1) {
        cout << "未设置指定 IP 和端口号，默认使用 127.0.0.1:3000" << endl;
    }
    else if (argc == 2) {
        cout << "IP : " << argv[1] << "，未设置指定端口号，默认使用 3000" << endl;
        IP = argv[1];
    }
    else if (argc == 3) {
        cout << "正在连接：" << argv[1] << ":" << argv[2] << endl;
        IP = argv[1];
        PORT = std::stoi(argv[2]); // 处理char* 转 int
    }

    ChatServer server(IP, PORT);
    ChatServer::serverInstance = &server;

    // 设置信号处理
    signal(SIGINT, ChatServer::SignalHandler);
    signal(SIGTERM, ChatServer::SignalHandler);

    server.Start();

    return 0;
}
