#define _CRT_SECURE_NO_WARNINGS
//2023211281-丁同勖-KMeans
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

int center[N];  // 判断每个点属于哪个簇

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
    "中国", "日本", "韩国", "伊朗",
    "沙特", "伊拉克", "卡塔尔", "阿联酋", "乌兹别克斯坦",
    "泰国", "越南", "阿曼", "巴林", "朝鲜", "印尼"
};

Point mean[K];  // 存储每个簇的中心点

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
    } // 找到每个坐标轴的最大值和最小值

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

// 欧几里得距离
float getDistance(const Point& point1, const Point& point2)
{
    return std::sqrt(
        (point1.x - point2.x) * (point1.x - point2.x) +
        (point1.y - point2.y) * (point1.y - point2.y) +
        (point1.z - point2.z) * (point1.z - point2.z)
    );
}

// 获取簇中心
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
            std::cout << "第 " << (i + 1) << " 簇的新中心点为：\t( "
                << meanArr[i].x << ", " << meanArr[i].y << " , " << meanArr[i].z << " )\n";
        }
        else
        {
            std::cout << "第 " << (i + 1) << " 簇没有分配到任何点。\n";
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
        std::cout << "\t" << std::left << std::setw(14) << Teams[i] << " 水平梯队 " << Rank << "\n";
    }
    std::cout << "-----------------------------------------\n\n";
}

int main()
{
    int n = 0;
    float temp1, temp2;

    std::cout << "\n----------------原始数据----------------\n\n";
    std::cout << "\033[47;30m" << std::setw(14) << "队伍"
        << std::setw(18) << "2006年世界杯"
        << std::setw(18) << "2010年世界杯"
        << std::setw(18) << "2007年亚洲杯" << "\033[0m\n";

    for (int i = 0; i < N; ++i)
    {
        std::cout << std::setw(14) << Teams[i]
            << std::setw(12) << point[i].x
            << std::setw(18) << point[i].y
            << std::setw(18) << point[i].z << "\n";
    }

    std::cout << "\n--------------归一化处理--------------\n";
    Normalization();

    // 初始化中心点
    mean[0] = point[1];   // 日本
    mean[1] = point[12];  // 巴林
    mean[2] = point[9];   // 泰国

    cluster();  // 运行聚类算法
    temp1 = getSSE();
    n++;

    std::cout << "第一次的 SSE（误差平方和）为: " << temp1 << "\n\n";

    getMean(center, mean);
    cluster();  // 运行聚类算法
    temp2 = getSSE(); // 检查 SSE
    n++;

    std::cout << "第二次的 SSE（误差平方和）为: " << temp2 << "\n\n";

    while (std::fabs(temp2 - temp1) != 0.0f) // 如果 SSE 不相等，则继续迭代
    {
        temp1 = temp2;
        getMean(center, mean);
        cluster();
        temp2 = getSSE();
        n++;
        std::cout << "第 " << n << " 次的 SSE（误差平方和）为: " << temp2 << "\n";
    }

    std::cout << "聚类迭代的总次数为: " << n << "\n\n";  // 统计迭代次数
    return 0;
}
