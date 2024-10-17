#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <cmath>
#include <thread>
#include <vector>
#include <string>
#include <iomanip>
#include "SignalAnalyze.h"

// ����ȫ�ֱ���
ASigInfo aSigInfo;
DSigInfo dSigInfo;
std::string StartupParameter = "";

// ��������
const std::string ActualSignalFileName = "data";
const std::string DeterministicSignalFileName = "PSS";
const std::string FileExtension = ".txt";
const std::string WorkLoadEnvDir = "D:/�����ļ�/��ҵ/����ѧ��_�����/ͨ���������/Fundamentals_Of_Communication_Software/task_1_SignalAnalyze/�γ�����/��һ��ʵ������/"; //�ź��ļ�·��

// ���� SortResult Ϊ��ά��������ƥ��ͷ�ļ��� SelectSort �Ĳ�������
std::vector<std::vector<double>> SortResult(2, std::vector<double>(MaxNumOfASigFiles, 0.0));

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
        if (!line.empty()) {
            fileLength++;
        }
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
    unsigned int currentFileLength;

    std::cout << InformationMsg << "��ʼ��ȡʵ���ź��ļ�.\n";

    for (unsigned int i = 0; i < MaxNumOfASigFiles; ++i) {
        _aSigInfo->FileCanBeOpen[i] = 0;

        std::string filePath = GetFileFullPath(ActualSignalFileName, i + ASigFileNameOffset, FileExtension);
        std::ifstream currentFile(filePath, std::ios::in);
        if (!currentFile.is_open()) {
            std::cout << WarningMsg << "�ļ� " << filePath << " ������.\n";
            continue;
        }

        currentFileLength = GetFileLength(currentFile);
        if (currentFileLength > MaxLenOfASig) {
            std::cout << ErrorMsg << "�ļ� " << filePath
                << " �ĳ��ȳ����� " << MaxLenOfASig << " ��������.\n";
            currentFile.close();
            continue;
        }

        _aSigInfo->PowerValue[i] = 0.0;

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
    unsigned int currentFileLength;

    std::cout << InformationMsg << "��ʼ��ȡȷ���ź��ļ�.\n";

    for (unsigned int i = 0; i < MaxNumOfDSigFiles; ++i) {
        _dSigInfo->FileCanBeOpen[i] = false;

        std::string filePath = GetFileFullPath(DeterministicSignalFileName, i + DSigFileNameOffset, FileExtension);
        std::ifstream currentFile(filePath, std::ios::in);
        if (!currentFile.is_open()) {
            std::cout << WarningMsg << "�ļ� " << filePath << " ������.\n";
            continue;
        }

        currentFileLength = GetFileLength(currentFile);
        if (currentFileLength > MaxLenOfDSig) {
            std::cout << ErrorMsg << "�ļ� " << filePath
                << " �ĳ��ȳ����� " << MaxLenOfDSig << " ��������.\n";
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
int SelectSort(const ASigInfo& aSigInfo, const DSigInfo& dSigInfo, std::vector<std::vector<double>>& sortResult) {
    // ��ʼ��������
    sortResult[0].resize(MaxNumOfASigFiles);
    sortResult[1].resize(MaxNumOfASigFiles);

    for (unsigned int index = 0; index < MaxNumOfASigFiles; ++index) {
        sortResult[0][index] = static_cast<double>(index);
        sortResult[1][index] = aSigInfo.PowerValue[index];
    }

    // ѡ�������㷨
    for (unsigned int i = 0; i < MaxNumOfASigFiles - 1; ++i) {
        unsigned int maxIdx = i;
        for (unsigned int j = i + 1; j < MaxNumOfASigFiles; ++j) {
            if (sortResult[1][j] > sortResult[1][maxIdx]) {
                maxIdx = j;
            }
        }
        // ����
        std::swap(sortResult[0][i], sortResult[0][maxIdx]);
        std::swap(sortResult[1][i], sortResult[1][maxIdx]);
    }

    // ���ǰ MaxNumOfSort �������ֵ
    std::cout << InformationMsg << "���� " << MaxNumOfSort << " ������ֵ����:\n";
    for (unsigned int index = 0; index < MaxNumOfSort; ++index) {
        std::cout << "\t" << std::setw(2) << (index + 1) << "|�ļ� "
            << static_cast<int>(sortResult[0][index]) + ASigFileNameOffset << "\t"
            << std::fixed << std::setprecision(6) << sortResult[1][index] << "\n";
    }
    return 0;
}

// ����Լ��
int CorrelationDetection(unsigned int aSigIndex, ASigInfo* _aSigInfo, unsigned int dSigIndex, DSigInfo* _dSigInfo) {
    // ��������Ƿ���Ч����ֹ��ֵ���±�Խ��
    if (aSigIndex < ASigFileNameOffset || dSigIndex < DSigFileNameOffset) {
        std::cout << ErrorMsg << "�����������¸�ֵ!\n";
        return -1;
    }

    unsigned int adjustedASigIndex = aSigIndex - ASigFileNameOffset;
    unsigned int adjustedDSigIndex = dSigIndex - DSigFileNameOffset;

    // ����ļ��Ƿ�ɷ���
    if (!_aSigInfo->FileCanBeOpen[adjustedASigIndex] || !_dSigInfo->FileCanBeOpen[adjustedDSigIndex]) {
        std::cout << ErrorMsg << "ʵ���ź��ļ���ȷ���ź��ļ����ɷ���!\n";
        return -1;
    }

    // ��ʵ���ź��ļ�
    std::string aSigFilePath = GetFileFullPath(ActualSignalFileName, aSigIndex, FileExtension);
    std::ifstream aSigFile(aSigFilePath, std::ios::in);
    if (!aSigFile.is_open()) {
        std::cout << WarningMsg << "�޷��� " << aSigFilePath << std::endl;
        return -1;
    }

    // ��ȷ���ź��ļ�
    std::string dSigFilePath = GetFileFullPath(DeterministicSignalFileName, dSigIndex, FileExtension);
    std::ifstream dSigFile(dSigFilePath, std::ios::in);
    if (!dSigFile.is_open()) {
        std::cout << WarningMsg << "�޷��� " << dSigFilePath << std::endl;
        aSigFile.close();
        return -1;
    }

    // ��ȡ�ļ�����
    unsigned int aSigFileLength = GetFileLength(aSigFile);
    unsigned int dSigFileLength = GetFileLength(dSigFile);

    // ��� aSigData �Ƿ����㹻�����ݽ�������Լ��
    if (aSigFileLength < dSigFileLength) {
        std::cout << ErrorMsg << "ʵ���ź��ļ� " << aSigFilePath << " �ĳ���С��ȷ���ź��ļ� " << dSigFilePath << ".\n";
        aSigFile.close();
        dSigFile.close();
        return -1;
    }

    // ��������ļ���
    std::string outputFileName = "CorrelationResult_Data" + std::to_string(aSigIndex) +
        "_PSS" + std::to_string(dSigIndex) + ".txt";

    StartupParameter += " " + outputFileName; // ׼�� Python ��ͼ����������

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
        if (!(aSigFile >> num.Re >> num.Im)) {
            std::cout << ErrorMsg << "��ȡʵ���ź��ļ� " << aSigFilePath << " ʱ����.\n";
            outputFile.close();
            aSigFile.close();
            dSigFile.close();
            return -1;
        }
    }

    // ��ȡȷ���ź�����
    std::vector<ComplexNum> dSigData(dSigFileLength / 2);
    for (auto& num : dSigData) {
        if (!(dSigFile >> num.Re >> num.Im)) {
            std::cout << ErrorMsg << "��ȡȷ���ź��ļ� " << dSigFilePath << " ʱ����.\n";
            outputFile.close();
            aSigFile.close();
            dSigFile.close();
            return -1;
        }
    }

    // �ر������ļ�
    aSigFile.close();
    dSigFile.close();

    double peakValue = 0.0;
    ComplexNum productValue;
    double productValueMod = 0.0;

    // �������ڽ�������Լ��
    size_t dSigSize = dSigData.size();
    size_t aSigSize = aSigData.size();

    for (size_t index = 0; index <= aSigSize - dSigSize; ++index) {
        productValue.Re = 0.0;
        productValue.Im = 0.0;

        for (size_t pos = 0; pos < dSigSize; ++pos) {
            CalculateComplexNumMultiplication(aSigData[index + pos], dSigData[pos], productValue);
        }

        productValueMod = std::sqrt(productValue.Re * productValue.Re + productValue.Im * productValue.Im);
        outputFile << productValueMod << "\n";

        if (peakValue < productValueMod) {
            peakValue = productValueMod;
            _aSigInfo->PeakValueIndex[adjustedASigIndex][adjustedDSigIndex] = static_cast<int>(index);
            _aSigInfo->PeakValue[adjustedASigIndex][adjustedDSigIndex] = productValueMod;
        }
    }

    // �ر�����ļ�
    outputFile.close();

    return 0;
}
