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

    // ���շ���˵ĳ�ʼ��Ϣ
    char buf[100] = { 0 };
    recv(socket_, buf, sizeof(buf), 0);
    cout << "�������Ϣ: " << buf << endl;

    // ���������߳�
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
        cout << "��������������: ";
        cin.getline(buf, sizeof(buf)); // ʹ��getline����������ո������

        if (send(socket_, buf, strlen(buf), 0) == SOCKET_ERROR) {
            cout << "������Ϣʧ��: " << GetLastError() << endl;
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
            cout << "\n�������Ϣ: " << buf << endl;
        }
        else {
            cout << "������Ϣʧ��: " << GetLastError() << endl;
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
        cout << "����: " << e.what() << endl;
    }
    return 0;
}
