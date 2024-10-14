#define _CRT_SECURE_NO_WARNINGS
//2023211281-��ͬ��-KMeans
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <cstring>
#include <string>

#define N 15
#define K 3

struct Point {
    float x;
    float y;
    float z;
};

int center[N];  // �ж�ÿ���������ĸ���

Point point[N] = {
    {50.0f, 50.0f, 9.0f},
    {28.0f, 9.0f, 4.0f},
    {17.0f, 15.0f, 3.0f},
    {25.0f, 40.0f, 5.0f},
    {28.0f, 40.0f, 2.0f},
    {50.0f, 50.0f, 1.0f},
    {50.0f, 40.0f, 9.0f},
    {50.0f, 40.0f, 9.0f},
    {40.0f, 40.0f, 5.0f},
    {50.0f, 50.0f, 9.0f},
    {50.0f, 50.0f, 5.0f},
    {50.0f, 50.0f, 9.0f},
    {40.0f, 40.0f, 9.0f},
    {40.0f, 32.0f, 17.0f},
    {50.0f, 50.0f, 9.0f},
};

const std::string Teams[N] = {
    "�й�", "�ձ�", "����", "����",
    "ɳ��", "������", "������", "������", "���ȱ��˹̹",
    "̩��", "Խ��", "����", "����", "����", "ӡ��"
};

Point mean[K];  // �洢ÿ���ص����ĵ�

void Normalization()
{
    Point maxPoint;
    Point minPoint;
    minPoint.x = minPoint.y = minPoint.z = 99999.0f;
    maxPoint.x = maxPoint.y = maxPoint.z = 0.0f;

    for (int i = 0; i < N; ++i)
    {
        if (maxPoint.x < point[i].x)
            maxPoint.x = point[i].x;
        if (maxPoint.y < point[i].y)
            maxPoint.y = point[i].y;
        if (maxPoint.z < point[i].z)
            maxPoint.z = point[i].z;
        if (minPoint.x > point[i].x)
            minPoint.x = point[i].x;
        if (minPoint.y > point[i].y)
            minPoint.y = point[i].y;
        if (minPoint.z > point[i].z)
            minPoint.z = point[i].z;
    } // �ҵ�ÿ������������ֵ����Сֵ

    for (int i = 0; i < N; ++i)
    {
        point[i].x = (point[i].x - minPoint.x) / (maxPoint.x - minPoint.x);
        point[i].y = (point[i].y - minPoint.y) / (maxPoint.y - minPoint.y);
        point[i].z = (point[i].z - minPoint.z) / (maxPoint.z - minPoint.z);
        std::cout << std::fixed << std::setprecision(2)
            << std::setw(12) << point[i].x
            << std::setw(18) << point[i].y
            << std::setw(18) << point[i].z << std::endl;
    }
}

// ŷ����þ���
float getDistance(const Point& point1, const Point& point2)
{
    return std::sqrt(
        (point1.x - point2.x) * (point1.x - point2.x) +
        (point1.y - point2.y) * (point1.y - point2.y) +
        (point1.z - point2.z) * (point1.z - point2.z)
    );
}

// ��ȡ������
void getMean(const int centerArr[N], Point meanArr[K])
{
    for (int i = 0; i < K; ++i)
    {
        Point temp = { 0.0f, 0.0f, 0.0f };
        int count = 0;
        for (int j = 0; j < N; ++j)
        {
            if (centerArr[j] == i)
            {
                count++;
                temp.x += point[j].x;
                temp.y += point[j].y;
                temp.z += point[j].z;
            }
        }
        if (count > 0)
        {
            temp.x /= count;
            temp.y /= count;
            temp.z /= count;
            meanArr[i] = temp;
            std::cout << "�� " << (i + 1) << " �ص������ĵ�Ϊ��\t( "
                << meanArr[i].x << ", " << meanArr[i].y << " , " << meanArr[i].z << " )\n";
        }
        else
        {
            std::cout << "�� " << (i + 1) << " ��û�з��䵽�κε㡣\n";
        }
    }
}

float getSSE()
{
    float sum = 0.0f;
    for (int i = 0; i < K; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            if (center[j] == i)
            {
                float dx = point[j].x - mean[i].x;
                float dy = point[j].y - mean[i].y;
                float dz = point[j].z - mean[i].z;
                sum += dx * dx + dy * dy + dz * dz;
            }
        }
    }
    return sum;
}

void cluster()
{
    float distance[N][K];

    for (int i = 0; i < N; ++i)
    {
        float minDist = std::numeric_limits<float>::max();
        for (int j = 0; j < K; ++j)
        {
            distance[i][j] = getDistance(point[i], mean[j]);
            if (distance[i][j] < minDist)
            {
                minDist = distance[i][j];
                center[i] = j;
            }
        }

        unsigned int Rank = 0;
        switch (center[i] + 1)
        {
        case 1: Rank = 1; break;
        case 2: Rank = 2; break;
        case 3: Rank = 3; break;
        default:
            break;
        }
        std::cout << "\t" << std::left << std::setw(14) << Teams[i] << " ˮƽ�ݶ� " << Rank << "\n";
    }
    std::cout << "-----------------------------------------\n\n";
}

int main()
{
    int n = 0;
    float temp1, temp2;

    std::cout << "\n----------------ԭʼ����----------------\n\n";
    std::cout << "\033[47;30m" << std::setw(14) << "����"
        << std::setw(18) << "2006�����籭"
        << std::setw(18) << "2010�����籭"
        << std::setw(18) << "2007�����ޱ�" << "\033[0m\n";

    for (int i = 0; i < N; ++i)
    {
        std::cout << std::setw(14) << Teams[i]
            << std::setw(12) << point[i].x
            << std::setw(18) << point[i].y
            << std::setw(18) << point[i].z << "\n";
    }

    std::cout << "\n--------------��һ������--------------\n";
    Normalization();

    // ��ʼ�����ĵ�
    mean[0] = point[1];   // �ձ�
    mean[1] = point[12];  // ����
    mean[2] = point[9];   // ̩��

    cluster();  // ���о����㷨
    temp1 = getSSE();
    n++;

    std::cout << "��һ�ε� SSE�����ƽ���ͣ�Ϊ: " << temp1 << "\n\n";

    getMean(center, mean);
    cluster();  // ���о����㷨
    temp2 = getSSE(); // ��� SSE
    n++;

    std::cout << "�ڶ��ε� SSE�����ƽ���ͣ�Ϊ: " << temp2 << "\n\n";

    while (std::fabs(temp2 - temp1) != 0.0f) // ��� SSE ����ȣ����������
    {
        temp1 = temp2;
        getMean(center, mean);
        cluster();
        temp2 = getSSE();
        n++;
        std::cout << "�� " << n << " �ε� SSE�����ƽ���ͣ�Ϊ: " << temp2 << "\n";
    }

    std::cout << "����������ܴ���Ϊ: " << n << "\n\n";  // ͳ�Ƶ�������
    return 0;
}
