#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <memory>
#include <string>
//2023211281-丁同勖-SOCKET_TCP_Client

using namespace std;

#pragma comment(lib, "ws2_32.lib")

class ChatClient {
public:
    ChatClient(const string& ipAddress, int port);
    ~ChatClient();
    void Start();

private:
    static DWORD WINAPI RecvMessage(LPVOID lpThread);
    void SendUsername();
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

    SendUsername(); // 发送用户名

    // 启动接收线程
    HANDLE hThread = CreateThread(NULL, 0, RecvMessage, (LPVOID)socket_, 0, NULL);
    if (hThread == NULL) {
        throw runtime_error("创建接收线程失败: " + to_string(GetLastError()));
    }
    CloseHandle(hThread);
}

ChatClient::~ChatClient() {
    running_ = false; // 设置为 false，以便停止接收消息
    closesocket(socket_); // 关闭套接字
    WSACleanup(); // 清理 Winsock
}

void ChatClient::Start() {
    SendMessage(); // 发送聊天消息
}

void ChatClient::SendUsername() {
    std::string username;
    std::cout << "请输入您的用户名: ";
    std::getline(std::cin, username);

    // 发送用户名到服务器
    if (send(socket_, username.c_str(), username.size(), 0) == SOCKET_ERROR) {
        std::cout << "发送用户名失败: " << GetLastError() << std::endl;
        return;
    }

    // 接收服务器的确认消息
    char buf[100] = { 0 };
    recv(socket_, buf, sizeof(buf), 0);
    std::cout << "服务器确认: " << buf << std::endl;
}

void ChatClient::SendMessage() {
    std::string message;
    while (running_) {
        cout << "请输入聊天内容: ";
        std::getline(std::cin, message); // 使用 getline 来处理包含空格的输入

        if (!message.empty()) { // 确保不发送空消息
            if (send(socket_, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
                std::cout << "发送消息失败: " << GetLastError() << std::endl;
                break; // 如果发送失败，退出循环
            }
            else {
                std::cout << "消息发送成功！" << std::endl;
            }
        }
        else {
            std::cout << "消息不能为空，请重新输入。" << std::endl;
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
            cout << "请输入聊天内容："; // 提示用户输入
        }
        else {
            cout << "接收消息失败: " << GetLastError() << endl;
            break; // 连接关闭或出错
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {

    string IP = "127.0.0.1";
    int PORT = 3000;

    if (argc == 1) {
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

    try {
        ChatClient client(IP, PORT);
        client.Start(); // 开始客户端
    }
    catch (const exception& e) {
        cout << "错误: " << e.what() << endl;
    }
    return 0;
}
