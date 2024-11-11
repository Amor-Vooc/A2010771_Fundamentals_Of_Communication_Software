#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <memory>
#include <string>
//2023211281-��ͬ��-SOCKET_TCP_Client

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

    SendUsername(); // �����û���

    // ���������߳�
    HANDLE hThread = CreateThread(NULL, 0, RecvMessage, (LPVOID)socket_, 0, NULL);
    if (hThread == NULL) {
        throw runtime_error("���������߳�ʧ��: " + to_string(GetLastError()));
    }
    CloseHandle(hThread);
}

ChatClient::~ChatClient() {
    running_ = false; // ����Ϊ false���Ա�ֹͣ������Ϣ
    closesocket(socket_); // �ر��׽���
    WSACleanup(); // ���� Winsock
}

void ChatClient::Start() {
    SendMessage(); // ����������Ϣ
}

void ChatClient::SendUsername() {
    std::string username;
    std::cout << "�����������û���: ";
    std::getline(std::cin, username);

    // �����û�����������
    if (send(socket_, username.c_str(), username.size(), 0) == SOCKET_ERROR) {
        std::cout << "�����û���ʧ��: " << GetLastError() << std::endl;
        return;
    }

    // ���շ�������ȷ����Ϣ
    char buf[100] = { 0 };
    recv(socket_, buf, sizeof(buf), 0);
    std::cout << "������ȷ��: " << buf << std::endl;
}

void ChatClient::SendMessage() {
    std::string message;
    while (running_) {
        cout << "��������������: ";
        std::getline(std::cin, message); // ʹ�� getline ����������ո������

        if (!message.empty()) { // ȷ�������Ϳ���Ϣ
            if (send(socket_, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
                std::cout << "������Ϣʧ��: " << GetLastError() << std::endl;
                break; // �������ʧ�ܣ��˳�ѭ��
            }
            else {
                std::cout << "��Ϣ���ͳɹ���" << std::endl;
            }
        }
        else {
            std::cout << "��Ϣ����Ϊ�գ����������롣" << std::endl;
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
            cout << "�������������ݣ�"; // ��ʾ�û�����
        }
        else {
            cout << "������Ϣʧ��: " << GetLastError() << endl;
            break; // ���ӹرջ����
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {

    string IP = "127.0.0.1";
    int PORT = 3000;

    if (argc == 1) {
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

    try {
        ChatClient client(IP, PORT);
        client.Start(); // ��ʼ�ͻ���
    }
    catch (const exception& e) {
        cout << "����: " << e.what() << endl;
    }
    return 0;
}
