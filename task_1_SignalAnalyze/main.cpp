#define _CRT_SECURE_NO_WARNINGS
#define __STDCPP_THREADS__ 1
//2023211281-丁同勖-信号相关性

#include <iostream>
#include <cmath>
#include <thread>
#include <vector>
#include <iomanip>
#include <chrono>
#include "SignalAnalyze.h"
#include "Message.h"


ASigInfo aSigInfo = {};
DSigInfo dSigInfo = {};
// SortResult[0][*] 存储索引，[1][*] 存储数据
double SortResult[2][MaxNumOfASigFiles] = { 0 };

// 函数声明
void CorrelationDetectionPerASig(unsigned int _aSigIndex);

int main() {
    // 多线程检查文件可用性
    std::thread LoadASigThread(ReadASigFile, &aSigInfo);
    std::thread LoadDSigThread(ReadDSigFile, &dSigInfo);

    LoadASigThread.join();
    LoadDSigThread.join();

    SelectSort(&aSigInfo, &dSigInfo, SortResult);

    // 多线程检查相关性
    std::vector<std::thread> CorrelationDetectionThreads;
    CorrelationDetectionThreads.reserve(MaxNumOfSort);

    for (unsigned int index = 0; index < MaxNumOfSort; ++index) {
        unsigned int aSigIndex = static_cast<unsigned int>(SortResult[0][index]) + ASigFileNameOffset;
        try {
            CorrelationDetectionThreads.emplace_back(CorrelationDetectionPerASig, aSigIndex);
        }
        catch (const std::system_error& e) {
            std::cerr << ErrorMsg << " 创建线程失败: " << e.what() << "\n";
        }
        std::this_thread::sleep_for(std::chrono::nanoseconds(5)); // 等待5纳秒
    }

    // 等待所有相关性线程结束
    for (auto& th : CorrelationDetectionThreads) {
        if (th.joinable()) {
            th.join();
            std::this_thread::sleep_for(std::chrono::nanoseconds(5)); // 等待5纳秒
        }
    }

    // 打印结果表头
    std::cout << "\n\033[47;30m"
        << std::setw(9) << "编号"
        << std::setw(7) << "数据"
        << std::setw(16) << "样本"
        << std::setw(17) << "相关性"
        << std::setw(13) << "位置"
        << "\033[0m\n";

    // 打印每条数据的相关性结果
    for (unsigned int index = 0; index < MaxNumOfSort; ++index) {
        unsigned int aSigIndex = static_cast<unsigned int>(SortResult[0][index]);
        for (unsigned int dSigIndex = 0; dSigIndex < MaxNumOfDSigFiles; ++dSigIndex) {
            std::cout << std::setw(8) << (index * 3 + dSigIndex + 1)
                << std::setw(14) << GetFileFullPathSetDir("", ActualSignalFileName, aSigIndex + ASigFileNameOffset, FileExtension)
                << std::setw(12) << GetFileFullPathSetDir("", DeterministicSignalFileName, dSigIndex + DSigFileNameOffset, FileExtension)
                << "\t" << std::fixed << std::setprecision(2) << aSigInfo.PeakValue[aSigIndex][dSigIndex]
                << "\t" << aSigInfo.PeakValueIndex[aSigIndex][dSigIndex] << "\n";
        }
        std::this_thread::sleep_for(std::chrono::nanoseconds(5)); // 等待5纳秒
    }

    // 计算每个采样信号的最大相关性
    double maxCorrelation[3][MaxNumOfDSigFiles];
    for (int i = 0; i < 3; ++i) {
        for (unsigned int j = 0; j < MaxNumOfDSigFiles; ++j) {
            maxCorrelation[i][j] = -1.0;
        }
    }

    for (unsigned int index = 0; index < MaxNumOfSort; ++index) {
        unsigned int aSigIndex = static_cast<unsigned int>(SortResult[0][index]);
        for (unsigned int dSigIndex = 0; dSigIndex < MaxNumOfDSigFiles; ++dSigIndex) {
            if (aSigInfo.PeakValue[aSigIndex][dSigIndex] > maxCorrelation[1][dSigIndex]) {
                maxCorrelation[1][dSigIndex] = aSigInfo.PeakValue[aSigIndex][dSigIndex];
                maxCorrelation[0][dSigIndex] = aSigIndex + ASigFileNameOffset;
                maxCorrelation[2][dSigIndex] = dSigIndex + DSigFileNameOffset;
            }
        }
    }

    // 打印最大相关性结果
    for (unsigned int dSigIndex = 0; dSigIndex < MaxNumOfDSigFiles; ++dSigIndex) {
        std::cout << InformationMsg << " 最大相关性存在于 "
            << GetFileFullPathSetDir("", ActualSignalFileName, static_cast<unsigned int>(maxCorrelation[0][dSigIndex]), FileExtension)
            << " 和 "
            << GetFileFullPathSetDir("", DeterministicSignalFileName, static_cast<unsigned int>(maxCorrelation[2][dSigIndex]), FileExtension)
            << " 之间 "
            << std::endl;
    }

    std::cout << InformationMsg << "将相关性输出保存到 CorrelationResult_Data*_PSS*.csv\n";
}

void CorrelationDetectionPerASig(unsigned int _aSigIndex) {
    for (unsigned int _dSigIndex = 0 + DSigFileNameOffset; _dSigIndex < MaxNumOfDSigFiles + DSigFileNameOffset; ++_dSigIndex) {
        CorrelationDetection(_aSigIndex, &aSigInfo, _dSigIndex, &dSigInfo);
    }
}
