#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <memory>
#include <string>
#include <windows.h>  // 用于改变控制台颜色
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
    static void SetConsoleColor(const string& colorName);  // 设置控制台颜色为静态函数
    static void ResetConsoleColor();  // 重置控制台颜色为静态函数

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
    int space = 1;
    while(space){ // 检查用户名是否有空格
        std::cout << "请输入您的用户名: ";
        std::getline(std::cin, username);
        if (username.find(' ') == string::npos) {
            space = 0;
        }
        else {
            cout << "用户名不能含有空格，请重新输入！" << endl;
        }
    }

    // 发送用户名到服务器
    if (send(socket_, username.c_str(), username.size(), 0) == SOCKET_ERROR) {
        std::cout << "发送用户名失败: " << GetLastError() << std::endl;
        return;
    }

    // 接收服务器的确认消息
    char buf[100] = { 0 };
    recv(socket_, buf, sizeof(buf), 0);
    std::cout << "服务器确认: " << buf << std::endl;
    std::cout << "Tip：#颜色 [消息] 可以改变消息颜色。支持的颜色有红色、绿色、蓝色、黄色、洋红和青色。\n" << std::endl;
}

void ChatClient::SendMessage() {
    std::string message;
    while (running_) {
        cout << "请输入聊天内容：";
        std::getline(std::cin, message); // 使用 getline 来处理包含空格的输入

        if (!message.empty()) { // 确保不发送空消息

            size_t spacePos = string::npos;
            spacePos = message.find(' '); // 获取颜色后的空格位置
            if (message[spacePos + 1] != '\0') { // 检查含颜色的输入是否为空
                if (send(socket_, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
                    cout << "发送消息失败: " << GetLastError() << endl;
                    break;
                }
                else {
                    cout << "消息发送成功！" << endl;
                }
            }
            else {
                cout << "消息不能为空，请重新输入。" << endl;
            }
        }
        else {
            cout << "消息不能为空，请重新输入。" << endl;
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
            string receivedMessage(buf, ret);

            // 清除未使用的输入提示并打印消息
            cout << "\033[1K\r";

            // 提取颜色和消息内容
            int count = 0;
            size_t spacePos1 = string::npos;
            size_t spacePos2 = string::npos;
            size_t end = string::npos;
            size_t startPos = 0;

            spacePos1 = receivedMessage.find(' ', startPos); // 获取用户名后的空格位置
            startPos = spacePos1 + 1; // 更新查找位置，防止重复查找

            spacePos2 = receivedMessage.find(' ', startPos); // 获取消息前的空格位置
            startPos = spacePos2 + 1; // 更新查找位置，防止重复查找

            end = receivedMessage.find('\0', startPos); // 获取消息结尾

            if (receivedMessage[spacePos1 + 5] == '#' && spacePos2 != string::npos) {
                string name= receivedMessage.substr(0, spacePos1 + 5); // 提取发送者用户名
                string colorCode = receivedMessage.substr(spacePos1+5, 5); // 识别颜色
                string msgContent = receivedMessage.substr(spacePos2 + 1, end - spacePos2 + 1); // 提取消息内容

                // 设置颜色
                SetConsoleColor(colorCode);
                cout << name;
                cout << msgContent << endl;

                // 重置颜色
                ResetConsoleColor();
            }
            else {
                cout << buf << endl;
            }

            cout << "请输入聊天内容："; // 提示用户输入

        }
        else {
            cout << "接收消息失败: " << GetLastError() << endl;
            break; // 连接关闭或出错
        }
    }
    return 0;
}

void ChatClient::SetConsoleColor(const string& colorCode) {
    // 根据颜色代码设置控制台颜色
    if (colorCode == "#红色") {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
    }
    else if (colorCode == "#绿色") {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }
    else if (colorCode == "#蓝色") {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    }
    else if (colorCode == "#黄色") {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }
    else if (colorCode == "#洋红") {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    }
    else if (colorCode == "#青色") {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    }
    else {
        // 默认颜色
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

void ChatClient::ResetConsoleColor() {
    // 重置为默认颜色
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

int main(int argc, char* argv[]) {

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

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
