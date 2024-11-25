#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <sstream>
#include <climits>
#include <iomanip>
//2023211281-��ͬ��-Link_State_Routing_Protocol


using namespace std;

const int INF = INT_MAX;
const int NODE_COUNT = 6; // ·������
const std::string ErrorMsg = "\033[31m[ERR]\033[0m\t";
const std::string InformationMsg = "\033[32m[INFO]\033[0m\t";
const std::string WarningMsg = "\033[33m[WARN]\033[0m\t";

void printIP(int index) {
    unsigned int Addr = 0xC0A80001 + index;
    cout << Addr / 256 / 256 / 256 << "." << Addr / 256 / 256 % 256 << "." << Addr / 256 % 256 << "." << Addr % 256;
}

class Network {
public:
    int adjMatrix[NODE_COUNT][NODE_COUNT];

    friend void printIP(int index);

    Network() {
        for (int i = 0; i < NODE_COUNT; ++i)
            for (int j = 0; j < NODE_COUNT; ++j)
                adjMatrix[i][j] = (i == j) ? 0 : INF; // �����ʼ����������������������Ϊ 0 �������Ϊ���ֵ
    }

    void loadFromFiles() {

        int src;
        for (src = 0; src < NODE_COUNT; ++src) {
            string fileName = "Router" + to_string(src) + ".txt";
            ifstream file(fileName);

            if (!file) {
                cerr << WarningMsg <<"�޷���ȡ�ļ� " << fileName << endl;
                continue;
            }

            cout << InformationMsg << "Router " << src << " ���ݺ鷺��ʼ" << endl;

            string line;
            while (getline(file, line)) {
                stringstream ss(line);
                int dest, weight;
                ss >> dest >> weight;

                if (src >= 0 && src < NODE_COUNT && dest >= 0 && dest < NODE_COUNT) {
                    adjMatrix[src][dest] = weight;
                }
                else {
                    cerr << ErrorMsg <<"�Ƿ����ݴ��� " << fileName << ": " << line << endl;
                }
            }
            file.close();
            cout << InformationMsg << "Router " << src << " ���ݺ鷺����" << endl;
        }
        cout << endl;
    }

    void printNetwork() {
        for (int i = 0; i < 6; ++i) {
            cout << InformationMsg << "Router " << i << " �� IP : ";
            printIP(i);
            cout << endl;
        }
        cout << endl << InformationMsg << "�ڽӾ���:\n" << endl;
        cout << "\033[47;30m" << setw(18) << "Router 0" << setw(10) << "Router 1" << setw(10) << "Router 2" << setw(10) << "Router 3" << setw(10) << "Router 4" << setw(10) << "Router 5" << "\033[47;0m\n";
        for (int i = 0; i < NODE_COUNT; ++i) {
            cout << "\033[47;30mRouter " << i << ":\033[47;0m" << setw(6) << left << "";
            for (int j = 0; j < NODE_COUNT; ++j) {
                if (adjMatrix[i][j] == INF)
                    cout << setw(9) << left << "INF";
                else
                    cout << setw(9) << left << adjMatrix[i][j];
            }
            cout << endl;
        }
        cout << endl;
    }
};

class Node {
public:
    int id;
    vector<int> distances;
    vector<int> predecessors;

    Node(int id) : id(id), distances(NODE_COUNT, INF), predecessors(NODE_COUNT, -1) {}

    friend void printIP(int index);

    void updateRoutingTable(Network& network) {
        distances.assign(NODE_COUNT, INF);
        predecessors.assign(NODE_COUNT, -1);
        distances[id] = 0;

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        pq.emplace(0, id);

        while (!pq.empty()) {
            int dist = pq.top().first;
            int current = pq.top().second;
            pq.pop();

            if (dist > distances[current]) continue;

            for (int neighbor = 0; neighbor < NODE_COUNT; ++neighbor) {
                if (network.adjMatrix[current][neighbor] != INF) {
                    int newDist = dist + network.adjMatrix[current][neighbor];
                    if (newDist < distances[neighbor]) {
                        distances[neighbor] = newDist;
                        predecessors[neighbor] = current;
                        pq.emplace(newDist, neighbor);
                    }
                }
            }
        }
    }

    void printRoutingTable() {
        cout << InformationMsg << "Router " << id << " ��·�ɱ�:\n";
        cout << "\033[47;30m" << setw(16) << left << "����" << setw(16) << left << "Ŀ��" << setw(8) << left << "����" << setw(14) << left << "��һ��" << "\033[47;0m\n";
        for (int i = 0; i < NODE_COUNT; ++i) {
            printIP(id);
            cout << "\t";
            printIP(i);
            cout << "\t" << (distances[i] == INF ? "INF" : to_string(distances[i])) << "\t";
            if (predecessors[i] == -1) {
                cout << "-" << endl;
            }
            else {
                printIP(predecessors[i]);
                cout << endl;
            }
        }
        cout << endl;
    }

    void sendData(int destination) {
        cout << InformationMsg << "Router " << id << " ������ Router " << destination << " ��������.\n";
        vector<int> path;
        int current = destination;

        while (current != id && current != -1) {
            path.push_back(current);
            current = predecessors[current];
        }

        if (current == -1) {
            cout << WarningMsg << "û�к��ʵ�·��!" << endl;
            return;
        }

        path.push_back(id);
        reverse(path.begin(), path.end());

        cout << InformationMsg << "����·��: ";
        for (auto& node : path) {
            printIP(node);
            cout << " -> ";
        }
        cout << "�ɹ�\n" << endl;
    }
};

int main() {
    Network network;

    vector<Node> nodes;
    for (int i = 0; i < NODE_COUNT; ++i) { // �½�·�ɽڵ㣬ģ�������еĸ���·��
        nodes.emplace_back(i);
    }

    network.loadFromFiles(); // ����鷺
    network.printNetwork(); // ����鷺����ڽӾ���

    // ���ݺ鷺�������·�ɱ�
    for (auto& node : nodes) {
        node.updateRoutingTable(network);
    }

    // ���·�ɱ�
    for (auto& node : nodes) {
        node.printRoutingTable();
    }

    // ���ݴ���·������
    while (1) {
        int src = -1, dest = -1;
        cout << InformationMsg << "��ʼ����·�ɼ�ͨ��·�����ԣ���������ʼ·�ɣ����� 0-5 ����";
        cin >> src;
        cout << InformationMsg << "������Ŀ��·�ɣ����� 0-5 ����";
        cin >> dest;
        if (src<6 && src>-1 && dest<6 && dest>-1)
            nodes[src].sendData(dest);
        else
            cout << WarningMsg << "·�ɷ�Χ����ȷ��\n" << endl;
    }
    return 0;
}