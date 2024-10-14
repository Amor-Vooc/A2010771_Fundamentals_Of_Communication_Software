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


DSigInfo dSigInfo = {};
SSigInfo sSigInfo = {};
// SortResult[0][*] store index, [1][*] store data
double SortResult[2][MaxNumOfDSigFiles] = { 0 };

// ��������
void CorrelationDetectionPerDSig(unsigned int _dSigIndex);

int main() {
    // ���̼߳���ļ�������
    std::thread LoadDSigThread(ReadDSigFile, &dSigInfo);
    std::thread LoadSSigThread(ReadSSigFile, &sSigInfo);

    LoadDSigThread.join();
    LoadSSigThread.join();

    SelectSort(&dSigInfo, &sSigInfo, SortResult);

    // ���̼߳�������
    std::vector<std::thread> CorrelationDetectionThreads;
    CorrelationDetectionThreads.reserve(MaxNumOfSort);

    for (unsigned int index = 0; index < MaxNumOfSort; ++index) {
        unsigned int dSigIndex = static_cast<unsigned int>(SortResult[0][index]) + DSigFileNameOffset;
        try {
            CorrelationDetectionThreads.emplace_back(CorrelationDetectionPerDSig, dSigIndex);
        }
        catch (const std::system_error& e) {
            std::cerr << ErrorMsg << " std::thread creation failed: " << e.what() << "\n";
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
        << std::setw(9) << "No."
        << std::setw(7) << "Data"
        << std::setw(16) << "Sample"
        << std::setw(20) << "Correlation"
        << std::setw(13) << "Position"
        << "\033[0m\n";

    // ��ӡÿ�����ݵ�����Խ��
    for (unsigned int index = 0; index < MaxNumOfSort; ++index) {
        unsigned int dSigIndex = static_cast<unsigned int>(SortResult[0][index]);
        for (unsigned int sSigIndex = 0; sSigIndex < MaxNumOfSSigFiles; ++sSigIndex) {
            std::cout << std::setw(8) << (index * 3 + sSigIndex + 1)
                << std::setw(14) << GetFileFullPathSetDir("", DeterministicSignalFileName, dSigIndex + DSigFileNameOffset, FileExtension)
                << std::setw(12) << GetFileFullPathSetDir("", SampleSignalFileName, sSigIndex + SSigFileNameOffset, FileExtension)
                << "\t" << std::fixed << std::setprecision(2) << dSigInfo.PeakValue[dSigIndex][sSigIndex]
                << "\t" << dSigInfo.PeakValueIndex[dSigIndex][sSigIndex] << "\n";
        }
        std::this_thread::sleep_for(std::chrono::nanoseconds(5)); // �ȴ�5����
    }

    // ����ÿ�������źŵ���������
    double maxCorrelation[3][MaxNumOfSSigFiles];
    for (int i = 0; i < 3; ++i) {
        for (unsigned int j = 0; j < MaxNumOfSSigFiles; ++j) {
            maxCorrelation[i][j] = -1.0;
        }
    }

    for (unsigned int index = 0; index < MaxNumOfSort; ++index) {
        unsigned int dSigIndex = static_cast<unsigned int>(SortResult[0][index]);
        for (unsigned int sSigIndex = 0; sSigIndex < MaxNumOfSSigFiles; ++sSigIndex) {
            if (dSigInfo.PeakValue[dSigIndex][sSigIndex] > maxCorrelation[1][sSigIndex]) {
                maxCorrelation[1][sSigIndex] = dSigInfo.PeakValue[dSigIndex][sSigIndex];
                maxCorrelation[0][sSigIndex] = dSigIndex + DSigFileNameOffset;
                maxCorrelation[2][sSigIndex] = sSigIndex + SSigFileNameOffset;
            }
        }
    }

    // ��ӡ�������Խ��
    for (unsigned int sSigIndex = 0; sSigIndex < MaxNumOfSSigFiles; ++sSigIndex) {
        std::cout << InformationMsg << "The max correlation exists between "
            << GetFileFullPathSetDir("", DeterministicSignalFileName, static_cast<unsigned int>(maxCorrelation[0][sSigIndex]), FileExtension)
            << " and "
            << GetFileFullPathSetDir("", SampleSignalFileName, static_cast<unsigned int>(maxCorrelation[2][sSigIndex]), FileExtension)
            << ".\n";
    }

    std::cout << InformationMsg << "Save Correlation output at CorrelationResult_Data*_PSS*.csv\n";
}

void CorrelationDetectionPerDSig(unsigned int _dSigIndex) {
    for (unsigned int _sSigIndex = 0 + SSigFileNameOffset; _sSigIndex < MaxNumOfSSigFiles + SSigFileNameOffset; ++_sSigIndex) {
        CorrelationDetection(_dSigIndex, &dSigInfo, _sSigIndex, &sSigInfo);
    }
}
