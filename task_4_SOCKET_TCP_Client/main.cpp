#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <memory>
#include <string>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

class ChatClient {
public:
    ChatClient(const string& ipAddress, int port);
    ~ChatClient();
    void Start();

private:
    static DWORD WINAPI RecvMessage(LPVOID lpThread);
    void SendMessage();

    SOCKET socket_;
    bool running_;
};

ChatClient::ChatClient(const string& ipAddress, int port) : running_(true) {
    WSADATA wd;
    if (WSAStartup(MAKEWORD(2, 2), &wd) != 0) {
        throw runtime_error("WSAStartup error: " + to_string(GetLastError()));
    }

    socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ == INVALID_SOCKET) {
        throw runtime_error("Socket error: " + to_string(GetLastError()));
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &addr.sin_addr);

    if (connect(socket_, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        throw runtime_error("Connect error: " + to_string(GetLastError()));
    }

    // 接收服务端的初始消息
    char buf[100] = { 0 };
    recv(socket_, buf, sizeof(buf), 0);
    cout << "服务端消息: " << buf << endl;

    // 启动接收线程
    HANDLE hThread = CreateThread(NULL, 0, RecvMessage, (LPVOID)socket_, 0, NULL);
    CloseHandle(hThread);
}

ChatClient::~ChatClient() {
    running_ = false;
    closesocket(socket_);
    WSACleanup();
}

void ChatClient::Start() {
    SendMessage();
}

void ChatClient::SendMessage() {
    char buf[100] = { 0 };
    while (running_) {
        cout << "请输入聊天内容: ";
        cin.getline(buf, sizeof(buf)); // 使用getline来处理包含空格的输入

        if (send(socket_, buf, strlen(buf), 0) == SOCKET_ERROR) {
            cout << "发送消息失败: " << GetLastError() << endl;
            break;
        }
    }
}

DWORD WINAPI ChatClient::RecvMessage(LPVOID lpThread) {
    SOCKET s = (SOCKET)lpThread;
    char buf[100] = { 0 };
    int ret = 0;

    while (true) {
        ret = recv(s, buf, sizeof(buf), 0);
        if (ret > 0) {
            cout << "\n服务端消息: " << buf << endl;
        }
        else {
            cout << "接收消息失败: " << GetLastError() << endl;
            break;
        }
    }
    return 0;
}

int main() {
    try {
        ChatClient client("127.0.0.1", 1145);
        client.Start();
    }
    catch (const exception& e) {
        cout << "错误: " << e.what() << endl;
    }
    return 0;
}
