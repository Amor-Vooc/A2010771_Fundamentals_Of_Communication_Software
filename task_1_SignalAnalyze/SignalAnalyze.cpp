#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <cmath>
#include <thread>
#include <vector>
#include <string>
#include <cstring>
#include <iomanip>
#include "SignalAnalyze.h"
#include "Message.h"

// ��������
const std::string ActualSignalFileName = "data";
const std::string DeterministicSignalFileName = "PSS";
const std::string FileExtension = ".txt";
const std::string WorkLoadEnvDir = "D:/�����ļ�/��ҵ/����ѧ��_�����/ͨ���������/Fundamentals_Of_Communication_Software/task_1_SignalAnalyze/�γ�����/��һ��ʵ������/"; //�ź��ļ�·��

const unsigned int StartOfAsigFiles = 40;
const unsigned int StartOfDSigFiles = 0;

extern ASigInfo aSigInfo;
extern DSigInfo dSigInfo;

extern double SortResult[2][MaxNumOfSort];


// ��ȡ�����Զ��幤��Ŀ¼���ļ�ȫ·��
std::string GetFileFullPathSetDir(const std::string& workLoadEnvDir, const std::string& fileClassName, unsigned int index, const std::string& extension) {
    return workLoadEnvDir + fileClassName + std::to_string(index) + extension;
}

// ��ȡ����Ŀ¼�е��ļ�ȫ·��
std::string GetFileFullPath(const std::string& fileClassName, unsigned int index, const std::string& extension) {
    return WorkLoadEnvDir + fileClassName + std::to_string(index) + extension;
}

// ��ȡ�ļ����ĳ��ȣ��������ƣ�
unsigned int GetFileLength(std::ifstream& fileStream) {
    unsigned int fileLength = 0;
    std::string line;
    if (!fileStream.is_open()) {
        return fileLength;
    }
    while (std::getline(fileStream, line)) {
        if (line.empty()) continue;
        fileLength++;
    }
    fileStream.clear(); // ���EOF��־
    fileStream.seekg(0, std::ios::beg); // �����ļ�ָ��
    return fileLength;
}

// ���㸴���˷�
int CalculateComplexNumMultiplication(const ComplexNum& multiplicand, const ComplexNum& multiplier, ComplexNum& product) {
    product.Re += (multiplicand.Re * multiplier.Re - multiplicand.Im * multiplier.Im);
    product.Im += (multiplicand.Im * multiplier.Re + multiplicand.Re * multiplier.Im);
    return 0;
}

// ��ȡʵ���ź��ļ��������书��
int ReadASigFile(ASigInfo* _aSigInfo) {
    ComplexNum currentComplexNum;
    std::ifstream currentFile;
    unsigned int currentFileLength;

    std::cout << InformationMsg << "��ʼ��ȡʵ���ź��ļ�.\n";

    for (int i = 0; i < MaxNumOfASigFiles; i++) {
        _aSigInfo->FileCanBeOpen[i] = 0;

        std::string filePath = GetFileFullPath(ActualSignalFileName, i + ASigFileNameOffset, FileExtension);
        currentFile.open(filePath, std::ios::in);
        if (!currentFile.is_open()) {
            std::cout << WarningMsg << "�ļ� " << filePath << " ������.\n";
            continue;
        }

        currentFileLength = GetFileLength(currentFile);
        if (currentFileLength > MaxLenOfASig) {
            std::cout << ErrorMsg << "�ļ� " << GetFileFullPath(ActualSignalFileName, i + ASigFileNameOffset, FileExtension)
                << "�ĳ��ȳ����� " << MaxLenOfASig << " ��������.\n";
            currentFile.close();
            continue;
        }

        _aSigInfo->PowerValue[i] = 0.0;
        currentComplexNum.Re = 0.0;
        currentComplexNum.Im = 0.0;

        // ���㹦��ֵ
        while (currentFile >> currentComplexNum.Re >> currentComplexNum.Im) {
            _aSigInfo->PowerValue[i] += std::sqrt(currentComplexNum.Re * currentComplexNum.Re + currentComplexNum.Im * currentComplexNum.Im);
        }

        std::cout << InformationMsg << "�ļ� " << filePath << " �Ĺ���ֵΪ " << std::fixed << std::setprecision(6)
            << _aSigInfo->PowerValue[i] << ".\n";
        _aSigInfo->FileCanBeOpen[i] = true;
        currentFile.close();
    }

    std::cout << InformationMsg << "��ȡʵ���ź��ļ����!\n";
    return 0;
}

// ��ȡȷ���ź��ļ�
int ReadDSigFile(DSigInfo* _dSigInfo) {
    std::ifstream currentFile;
    unsigned int currentFileLength;

    std::cout << InformationMsg << "��ʼ��ȡȷ���ź��ļ�.\n";

    for (int i = 0; i < MaxNumOfDSigFiles; i++) {
        _dSigInfo->FileCanBeOpen[i] = false;

        std::string filePath = GetFileFullPath(DeterministicSignalFileName, i + DSigFileNameOffset, FileExtension);
        currentFile.open(filePath, std::ios::in);
        if (!currentFile.is_open()) {
            std::cout << WarningMsg << "�ļ� " << filePath << " ������.\n";
            continue;
        }

        currentFileLength = GetFileLength(currentFile);
        if (currentFileLength > MaxLenOfDSig) {
            std::cout << ErrorMsg << "�ļ� " << GetFileFullPath(DeterministicSignalFileName, i + DSigFileNameOffset, FileExtension)
                << "�ĳ��ȳ����� " << MaxLenOfDSig << " ��������.\n";
            currentFile.close();
            continue;
        }

        _dSigInfo->FileCanBeOpen[i] = true;
        currentFile.close();
    }

    std::cout << InformationMsg << "��ȡȷ���ź��ļ����!\n";
    return 0;
}

// ѡ�������ҵ������ֵ������
int SelectSort(ASigInfo* _aSigInfo, DSigInfo* _dSigInfo, double(*sortResult)[MaxNumOfASigFiles]) {
    int maxIndex = 0;
    maxIndex = 0;
    // ��ʼ��������
    for (int index = 0; index < MaxNumOfASigFiles; index++) {
        sortResult[0][index] = static_cast<double>(index);
        sortResult[1][index] = _aSigInfo->PowerValue[index];
    }

    // ѡ�������㷨
    for (int index = 0; index < MaxNumOfASigFiles - 1; index++) {
        int maxIndex = index;
        for (int j = index + 1; j < MaxNumOfASigFiles; j++) {
            if (sortResult[1][j] > sortResult[1][maxIndex]) {
                maxIndex = j;
            }
        }
        // ����
        std::swap(sortResult[0][maxIndex], sortResult[0][index]);
        std::swap(sortResult[1][maxIndex], sortResult[1][index]);
    }

    // ���ǰMaxNumOfSort�������ֵ
    std::cout << InformationMsg << "���� " << MaxNumOfSort << " ������ֵ����:\n";
    for (int index = 0; index < MaxNumOfSort; index++) {
        std::cout << "\t" << std::setw(2) << (index + 1) << "|�ļ� "
            << static_cast<int>(sortResult[0][index]) + ASigFileNameOffset << "\t"
            << std::fixed << std::setprecision(6) << sortResult[1][index] << "\n";
    }

    return 0;
}

// ����Լ��
int CorrelationDetection(unsigned int aSigIndex, ASigInfo* _aSigInfo, unsigned int dSigIndex, DSigInfo* _dSigInfo) {
    // ��������
    aSigIndex -= ASigFileNameOffset;
    dSigIndex -= DSigFileNameOffset;

    // ����ļ��Ƿ�ɷ���
    if (!_aSigInfo->FileCanBeOpen[aSigIndex] || !_dSigInfo->FileCanBeOpen[dSigIndex]) {
        std::cout << ErrorMsg << "ʵ���ź��ļ���ȷ���ź��ļ����ɷ���!\n";
        return -1;
    }

    // ��ʵ���ź��ļ�
    std::string aSigFilePath = GetFileFullPath(ActualSignalFileName, aSigIndex + ASigFileNameOffset, FileExtension);
    std::ifstream aSigFile(aSigFilePath, std::ios::in);
    if (!aSigFile.is_open()) {
        std::cout << WarningMsg << "�޷��� " << aSigFilePath << std::endl;
        return -1;
    }

    // ��ȷ���ź��ļ�
    std::string dSigFilePath = GetFileFullPath(DeterministicSignalFileName, dSigIndex + DSigFileNameOffset, FileExtension);
    std::ifstream dSigFile(dSigFilePath, std::ios::in);
    if (!dSigFile.is_open()) {
        std::cout << WarningMsg << "�޷��� " << dSigFilePath << std::endl;
        aSigFile.close();
        return -1;
    }

    // ��ȡ�ļ�����
    unsigned int aSigFileLength = GetFileLength(aSigFile);
    unsigned int dSigFileLength = GetFileLength(dSigFile);

    // ��������ļ���
    std::string outputFileName = "CorrelationResult_Data" + std::to_string(aSigIndex + ASigFileNameOffset) +
        "_PSS" + std::to_string(dSigIndex + DSigFileNameOffset) + ".csv";
    std::ofstream outputFile(outputFileName, std::ios::out);
    if (!outputFile.is_open()) {
        std::cout << WarningMsg << "����ʧ�� " << outputFileName << std::endl;
        aSigFile.close();
        dSigFile.close();
        return -1;
    }

    // ��ȡʵ���ź�����
    std::vector<ComplexNum> aSigData(aSigFileLength / 2);
    for (auto& num : aSigData) {
        aSigFile >> num.Re >> num.Im;
    }

    // ��ȡȷ���ź�����
    std::vector<ComplexNum> dSigData(dSigFileLength / 2);
    for (auto& num : dSigData) {
        dSigFile >> num.Re >> num.Im;
    }

    // �ر������ļ�
    aSigFile.close();
    dSigFile.close();

    double peakValue = 0.0;
    ComplexNum productValue = { 0.0, 0.0 };
    double productValueMod = 0.0;

    // �������ڽ�������Լ��
    for (unsigned int index = 0; index <= aSigData.size() - dSigData.size(); ++index) {
        productValue.Re = 0.0;
        productValue.Im = 0.0;

        for (size_t position = 0; position < dSigData.size(); ++position) {
            CalculateComplexNumMultiplication(aSigData[index + position], dSigData[position], productValue);
        }

        productValueMod = std::sqrt(productValue.Re * productValue.Re + productValue.Im * productValue.Im);
        outputFile << productValueMod << "\n";

        if (peakValue < productValueMod) {
            peakValue = productValueMod;
            _aSigInfo->PeakValueIndex[aSigIndex][dSigIndex] = index;
            _aSigInfo->PeakValue[aSigIndex][dSigIndex] = productValueMod;
        }
    }

    // �ر�����ļ�
    outputFile.close();

    return 0;
}
