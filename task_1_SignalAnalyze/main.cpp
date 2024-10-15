#define _CRT_SECURE_NO_WARNINGS
#define __STDCPP_THREADS__ 1
//2023211281-��ͬ��-�ź������

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
// SortResult[0][*] �洢������[1][*] �洢����
double SortResult[2][MaxNumOfASigFiles] = { 0 };

// ��������
void CorrelationDetectionPerASig(unsigned int _aSigIndex);

int main() {
    // ���̼߳���ļ�������
    std::thread LoadASigThread(ReadASigFile, &aSigInfo);
    std::thread LoadDSigThread(ReadDSigFile, &dSigInfo);

    LoadASigThread.join();
    LoadDSigThread.join();

    SelectSort(&aSigInfo, &dSigInfo, SortResult);

    // ���̼߳�������
    std::vector<std::thread> CorrelationDetectionThreads;
    CorrelationDetectionThreads.reserve(MaxNumOfSort);

    for (unsigned int index = 0; index < MaxNumOfSort; ++index) {
        unsigned int aSigIndex = static_cast<unsigned int>(SortResult[0][index]) + ASigFileNameOffset;
        try {
            CorrelationDetectionThreads.emplace_back(CorrelationDetectionPerASig, aSigIndex);
        }
        catch (const std::system_error& e) {
            std::cerr << ErrorMsg << " �����߳�ʧ��: " << e.what() << "\n";
        }
        std::this_thread::sleep_for(std::chrono::nanoseconds(5)); // �ȴ�5����
    }

    // �ȴ�����������߳̽���
    for (auto& th : CorrelationDetectionThreads) {
        if (th.joinable()) {
            th.join();
            std::this_thread::sleep_for(std::chrono::nanoseconds(5)); // �ȴ�5����
        }
    }

    // ��ӡ�����ͷ
    std::cout << "\n\033[47;30m"
        << std::setw(9) << "���"
        << std::setw(7) << "����"
        << std::setw(16) << "����"
        << std::setw(17) << "�����"
        << std::setw(13) << "λ��"
        << "\033[0m\n";

    // ��ӡÿ�����ݵ�����Խ��
    for (unsigned int index = 0; index < MaxNumOfSort; ++index) {
        unsigned int aSigIndex = static_cast<unsigned int>(SortResult[0][index]);
        for (unsigned int dSigIndex = 0; dSigIndex < MaxNumOfDSigFiles; ++dSigIndex) {
            std::cout << std::setw(8) << (index * 3 + dSigIndex + 1)
                << std::setw(14) << GetFileFullPathSetDir("", ActualSignalFileName, aSigIndex + ASigFileNameOffset, FileExtension)
                << std::setw(12) << GetFileFullPathSetDir("", DeterministicSignalFileName, dSigIndex + DSigFileNameOffset, FileExtension)
                << "\t" << std::fixed << std::setprecision(2) << aSigInfo.PeakValue[aSigIndex][dSigIndex]
                << "\t" << aSigInfo.PeakValueIndex[aSigIndex][dSigIndex] << "\n";
        }
        std::this_thread::sleep_for(std::chrono::nanoseconds(5)); // �ȴ�5����
    }

    // ����ÿ�������źŵ���������
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
}

void CorrelationDetectionPerASig(unsigned int _aSigIndex) {
    for (unsigned int _dSigIndex = 0 + DSigFileNameOffset; _dSigIndex < MaxNumOfDSigFiles + DSigFileNameOffset; ++_dSigIndex) {
        CorrelationDetection(_aSigIndex, &aSigInfo, _dSigIndex, &dSigInfo);
    }
}
