#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <memory>
#include <string>
#include <windows.h>  // ���ڸı����̨��ɫ
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
    static void SetConsoleColor(const string& colorName);  // ���ÿ���̨��ɫΪ��̬����
    static void ResetConsoleColor();  // ���ÿ���̨��ɫΪ��̬����

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
    int space = 1;
    while(space){ // ����û����Ƿ��пո�
        std::cout << "�����������û���: ";
        std::getline(std::cin, username);
        if (username.find(' ') == string::npos) {
            space = 0;
        }
        else {
            cout << "�û������ܺ��пո����������룡" << endl;
        }
    }

    // �����û�����������
    if (send(socket_, username.c_str(), username.size(), 0) == SOCKET_ERROR) {
        std::cout << "�����û���ʧ��: " << GetLastError() << std::endl;
        return;
    }

    // ���շ�������ȷ����Ϣ
    char buf[100] = { 0 };
    recv(socket_, buf, sizeof(buf), 0);
    std::cout << "������ȷ��: " << buf << std::endl;
    std::cout << "Tip��#��ɫ [��Ϣ] ���Ըı���Ϣ��ɫ��֧�ֵ���ɫ�к�ɫ����ɫ����ɫ����ɫ��������ɫ��\n" << std::endl;
}

void ChatClient::SendMessage() {
    std::string message;
    while (running_) {
        cout << "�������������ݣ�";
        std::getline(std::cin, message); // ʹ�� getline ����������ո������

        if (!message.empty()) { // ȷ�������Ϳ���Ϣ

            size_t spacePos = string::npos;
            spacePos = message.find(' '); // ��ȡ��ɫ��Ŀո�λ��
            if (message[spacePos + 1] != '\0') { // ��麬��ɫ�������Ƿ�Ϊ��
                if (send(socket_, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
                    cout << "������Ϣʧ��: " << GetLastError() << endl;
                    break;
                }
                else {
                    cout << "��Ϣ���ͳɹ���" << endl;
                }
            }
            else {
                cout << "��Ϣ����Ϊ�գ����������롣" << endl;
            }
        }
        else {
            cout << "��Ϣ����Ϊ�գ����������롣" << endl;
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

            // ���δʹ�õ�������ʾ����ӡ��Ϣ
            cout << "\033[1K\r";

            // ��ȡ��ɫ����Ϣ����
            int count = 0;
            size_t spacePos1 = string::npos;
            size_t spacePos2 = string::npos;
            size_t end = string::npos;
            size_t startPos = 0;

            spacePos1 = receivedMessage.find(' ', startPos); // ��ȡ�û�����Ŀո�λ��
            startPos = spacePos1 + 1; // ���²���λ�ã���ֹ�ظ�����

            spacePos2 = receivedMessage.find(' ', startPos); // ��ȡ��Ϣǰ�Ŀո�λ��
            startPos = spacePos2 + 1; // ���²���λ�ã���ֹ�ظ�����

            end = receivedMessage.find('\0', startPos); // ��ȡ��Ϣ��β

            if (receivedMessage[spacePos1 + 5] == '#' && spacePos2 != string::npos) {
                string name= receivedMessage.substr(0, spacePos1 + 5); // ��ȡ�������û���
                string colorCode = receivedMessage.substr(spacePos1+5, 5); // ʶ����ɫ
                string msgContent = receivedMessage.substr(spacePos2 + 1, end - spacePos2 + 1); // ��ȡ��Ϣ����

                // ������ɫ
                SetConsoleColor(colorCode);
                cout << name;
                cout << msgContent << endl;

                // ������ɫ
                ResetConsoleColor();
            }
            else {
                cout << buf << endl;
            }

            cout << "�������������ݣ�"; // ��ʾ�û�����

        }
        else {
            cout << "������Ϣʧ��: " << GetLastError() << endl;
            break; // ���ӹرջ����
        }
    }
    return 0;
}

void ChatClient::SetConsoleColor(const string& colorCode) {
    // ������ɫ�������ÿ���̨��ɫ
    if (colorCode == "#��ɫ") {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
    }
    else if (colorCode == "#��ɫ") {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }
    else if (colorCode == "#��ɫ") {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    }
    else if (colorCode == "#��ɫ") {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }
    else if (colorCode == "#���") {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    }
    else if (colorCode == "#��ɫ") {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    }
    else {
        // Ĭ����ɫ
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

void ChatClient::ResetConsoleColor() {
    // ����ΪĬ����ɫ
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

int main(int argc, char* argv[]) {

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

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
