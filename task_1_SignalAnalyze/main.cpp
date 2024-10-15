#define _CRT_SECURE_NO_WARNINGS
#define __STDCPP_THREADS__ 1
//2023211281-��ͬ��-�ź������

#include <iostream>
#include <cmath>
#include <thread>
#include <vector>
#include <iomanip>
#include <chrono>
#include <future>
#include "SignalAnalyze.h"
#include "Message.h"

// ʹ�������ռ��Ա���ȫ�ֱ���
struct SignalData {
    ASigInfo aSigInfo = {};
    DSigInfo dSigInfo = {};
    // ʹ�� std::vector ����ԭ������
    std::vector<std::vector<double>> SortResult = { std::vector<double>(MaxNumOfASigFiles, 0.0),
                                                   std::vector<double>(MaxNumOfASigFiles, 0.0) };
};

// ��������
void CorrelationDetectionPerASig(unsigned int aSigIndex, SignalData& signalData);

int main() {
    SignalData signalData;

    // ���̼߳���ļ�������
    std::thread loadASigThread(ReadASigFile, &signalData.aSigInfo);
    std::thread loadDSigThread(ReadDSigFile, &signalData.dSigInfo);

    loadASigThread.join();
    loadDSigThread.join();

    SelectSort(signalData.aSigInfo, signalData.dSigInfo, signalData.SortResult);

    // ʹ���첽��������ֶ��̹߳���
    std::vector<std::future<void>> correlationFutures;
    correlationFutures.reserve(MaxNumOfSort);

    for (unsigned int index = 0; index < MaxNumOfSort; ++index) {
        unsigned int aSigIndex = static_cast<unsigned int>(signalData.SortResult[0][index]) + ASigFileNameOffset;
        correlationFutures.emplace_back(std::async(std::launch::async, CorrelationDetectionPerASig, aSigIndex, std::ref(signalData)));
    }

    // �ȴ�����������߳̽���
    for (auto& future : correlationFutures) {
        try {
            future.get();
        }
        catch (const std::exception& e) {
            std::cerr << ErrorMsg << " �߳�ִ��ʧ��: " << e.what() << "\n";
        }
    }

    // ��ӡ�����ͷ
    std::cout << "\n\033[47;30m"
        << std::setw(9) << "���"
        << std::setw(10) << "����"
        << std::setw(14) << "����"
        << std::setw(16) << "�����"
        << std::setw(12) << "λ��"
        << "\033[0m\n";

    // ��ӡÿ�����ݵ�����Խ��
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

    // ����ÿ�������źŵ���������
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

    // ��ӡ�������Խ��
    for (unsigned int dSigIndex = 0; dSigIndex < MaxNumOfDSigFiles; ++dSigIndex) {
        std::cout << InformationMsg << " �������Դ����� "
            << GetFileFullPathSetDir("", ActualSignalFileName, static_cast<unsigned int>(maxCorrelation[0][dSigIndex]), FileExtension)
            << " �� "
            << GetFileFullPathSetDir("", DeterministicSignalFileName, static_cast<unsigned int>(maxCorrelation[2][dSigIndex]), FileExtension)
            << " ֮�� "
            << std::endl;
    }

    std::cout << InformationMsg << "�������������浽 CorrelationResult_Data*_PSS*.csv\n";

    return 0;
}

void CorrelationDetectionPerASig(unsigned int aSigIndex, SignalData& signalData) {
    for (unsigned int dSigIndex = 0; dSigIndex < MaxNumOfDSigFiles; ++dSigIndex) {
        CorrelationDetection(aSigIndex, &signalData.aSigInfo, dSigIndex + DSigFileNameOffset, &signalData.dSigInfo);
    }
}
