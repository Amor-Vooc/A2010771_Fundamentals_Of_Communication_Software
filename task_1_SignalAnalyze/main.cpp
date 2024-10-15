#define _CRT_SECURE_NO_WARNINGS
#define __STDCPP_THREADS__ 1
//2023211281-丁同勖-信号相关性

#include <iostream>
#include <cmath>
#include <thread>
#include <vector>
#include <iomanip>
#include <chrono>
#include <future>
#include "SignalAnalyze.h"
#include "Message.h"

// 使用命名空间以避免全局变量
struct SignalData {
    ASigInfo aSigInfo = {};
    DSigInfo dSigInfo = {};
    // 使用 std::vector 代替原生数组
    std::vector<std::vector<double>> SortResult = { std::vector<double>(MaxNumOfASigFiles, 0.0),
                                                   std::vector<double>(MaxNumOfASigFiles, 0.0) };
};

// 函数声明
void CorrelationDetectionPerASig(unsigned int aSigIndex, SignalData& signalData);

int main() {
    SignalData signalData;

    // 多线程检查文件可用性
    std::thread loadASigThread(ReadASigFile, &signalData.aSigInfo);
    std::thread loadDSigThread(ReadDSigFile, &signalData.dSigInfo);

    loadASigThread.join();
    loadDSigThread.join();

    SelectSort(signalData.aSigInfo, signalData.dSigInfo, signalData.SortResult);

    // 使用异步任务代替手动线程管理
    std::vector<std::future<void>> correlationFutures;
    correlationFutures.reserve(MaxNumOfSort);

    for (unsigned int index = 0; index < MaxNumOfSort; ++index) {
        unsigned int aSigIndex = static_cast<unsigned int>(signalData.SortResult[0][index]) + ASigFileNameOffset;
        correlationFutures.emplace_back(std::async(std::launch::async, CorrelationDetectionPerASig, aSigIndex, std::ref(signalData)));
    }

    // 等待所有相关性线程结束
    for (auto& future : correlationFutures) {
        try {
            future.get();
        }
        catch (const std::exception& e) {
            std::cerr << ErrorMsg << " 线程执行失败: " << e.what() << "\n";
        }
    }

    // 打印结果表头
    std::cout << "\n\033[47;30m"
        << std::setw(9) << "编号"
        << std::setw(10) << "数据"
        << std::setw(14) << "样本"
        << std::setw(16) << "相关性"
        << std::setw(12) << "位置"
        << "\033[0m\n";

    // 打印每条数据的相关性结果
    for (unsigned int index = 0; index < MaxNumOfSort; ++index) {
        unsigned int aSigIndex = static_cast<unsigned int>(signalData.SortResult[0][index]);
        for (unsigned int dSigIndex = 0; dSigIndex < MaxNumOfDSigFiles; ++dSigIndex) {
            std::cout << std::setw(8) << (index * 3 + dSigIndex + 1)
                << std::setw(14) << GetFileFullPathSetDir("", ActualSignalFileName, aSigIndex + ASigFileNameOffset, FileExtension)
                << std::setw(12) << GetFileFullPathSetDir("", DeterministicSignalFileName, dSigIndex + DSigFileNameOffset, FileExtension)
                << "\t" << std::fixed << std::setprecision(2) << signalData.aSigInfo.PeakValue[aSigIndex][dSigIndex]
                << "\t" << signalData.aSigInfo.PeakValueIndex[aSigIndex][dSigIndex] << "\n";
        }
    }

    // 计算每个采样信号的最大相关性
    std::vector<std::vector<double>> maxCorrelation(3, std::vector<double>(MaxNumOfDSigFiles, -1.0));

    for (unsigned int index = 0; index < MaxNumOfSort; ++index) {
        unsigned int aSigIndex = static_cast<unsigned int>(signalData.SortResult[0][index]);
        for (unsigned int dSigIndex = 0; dSigIndex < MaxNumOfDSigFiles; ++dSigIndex) {
            double currentCorrelation = signalData.aSigInfo.PeakValue[aSigIndex][dSigIndex];
            if (currentCorrelation > maxCorrelation[1][dSigIndex]) {
                maxCorrelation[1][dSigIndex] = currentCorrelation;
                maxCorrelation[0][dSigIndex] = static_cast<double>(aSigIndex + ASigFileNameOffset);
                maxCorrelation[2][dSigIndex] = static_cast<double>(dSigIndex + DSigFileNameOffset);
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

    return 0;
}

void CorrelationDetectionPerASig(unsigned int aSigIndex, SignalData& signalData) {
    for (unsigned int dSigIndex = 0; dSigIndex < MaxNumOfDSigFiles; ++dSigIndex) {
        CorrelationDetection(aSigIndex, &signalData.aSigInfo, dSigIndex + DSigFileNameOffset, &signalData.dSigInfo);
    }
}
