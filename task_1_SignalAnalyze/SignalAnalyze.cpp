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

// 常量定义
const std::string DeterministicSignalFileName = "data";
const std::string SampleSignalFileName = "PSS";
const std::string FileExtension = ".txt";
const std::string WorkLoadEnvDir = "D:/个人文件/作业/第三学期_大二上/通信软件基础/Fundamentals_Of_Communication_Software/task_1_SignalAnalyze/课程资料/第一次实验数据/"; //信号文件路径

const unsigned int StartOfDsigFiles = 40;
const unsigned int StartOfSSigFiles = 0;

extern DSigInfo dSigInfo;
extern SSigInfo sSigInfo;

extern double SortResult[2][MaxNumOfSort];


// 获取带有自定义工作目录的文件全路径
std::string GetFileFullPathSetDir(const std::string& workLoadEnvDir, const std::string& fileClassName, unsigned int index, const std::string& extension) {
    return workLoadEnvDir + fileClassName + std::to_string(index) + extension;
}

// 获取工作目录中的文件全路径
std::string GetFileFullPath(const std::string& fileClassName, unsigned int index, const std::string& extension) {
    return WorkLoadEnvDir + fileClassName + std::to_string(index) + extension;
}

// 获取文件流的长度（以行数计）
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
    fileStream.clear(); // 清除EOF标志
    fileStream.seekg(0, std::ios::beg); // 重置文件指针
    return fileLength;
}

// 计算复数乘法
int CalculateComplexNumMultiplication(const ComplexNum& multiplicand, const ComplexNum& multiplier, ComplexNum& product) {
    product.Re += (multiplicand.Re * multiplier.Re - multiplicand.Im * multiplier.Im);
    product.Im += (multiplicand.Im * multiplier.Re + multiplicand.Re * multiplier.Im);
    return 0;
}

// 读取确定性信号文件并计算其功率
int ReadDSigFile(DSigInfo* _dSigInfo) {
    ComplexNum currentComplexNum;
    std::ifstream currentFile;
    unsigned int currentFileLength;

    std::cout << InformationMsg << "Start reading deterministic signal file.\n";

    for (int i = 0; i < MaxNumOfDSigFiles; i++) {
        _dSigInfo->FileCanBeOpen[i] = 0;

        std::string filePath = GetFileFullPath(DeterministicSignalFileName, i + DSigFileNameOffset, FileExtension);
        currentFile.open(filePath, std::ios::in);
        if (!currentFile.is_open()) {
            std::cout << WarningMsg << "File " << filePath << " does not exist.\n";
            continue;
        }

        currentFileLength = GetFileLength(currentFile);
        if (currentFileLength > MaxLenOfDSig) {
            std::cout << ErrorMsg << "File " << GetFileFullPath(DeterministicSignalFileName, i + DSigFileNameOffset, FileExtension)
                << "'s length exceeds the " << MaxLenOfDSig << " line limit.\n";
            currentFile.close();
            continue;
        }

        _dSigInfo->PowerValue[i] = 0.0;
        currentComplexNum.Re = 0.0;
        currentComplexNum.Im = 0.0;

        // 计算功率值
        while (currentFile >> currentComplexNum.Re >> currentComplexNum.Im) {
            _dSigInfo->PowerValue[i] += std::sqrt(currentComplexNum.Re * currentComplexNum.Re + currentComplexNum.Im * currentComplexNum.Im);
        }

        std::cout << InformationMsg << "The power strength of " << filePath << " is " << std::fixed << std::setprecision(6)
            << _dSigInfo->PowerValue[i] << ".\n";
        _dSigInfo->FileCanBeOpen[i] = true;
        currentFile.close();
    }

    std::cout << InformationMsg << "Read deterministic signal file done!\n";
    return 0;
}

// 读取采样信号文件
int ReadSSigFile(SSigInfo* _sSigInfo) {
    std::ifstream currentFile;
    unsigned int currentFileLength;

    std::cout << InformationMsg << "Start reading sample signal file.\n";

    for (int i = 0; i < MaxNumOfSSigFiles; i++) {
        _sSigInfo->FileCanBeOpen[i] = false;

        std::string filePath = GetFileFullPath(SampleSignalFileName, i + SSigFileNameOffset, FileExtension);
        currentFile.open(filePath, std::ios::in);
        if (!currentFile.is_open()) {
            std::cout << WarningMsg << "File " << filePath << " does not exist.\n";
            continue;
        }

        currentFileLength = GetFileLength(currentFile);
        if (currentFileLength > MaxLenOfSSig) {
            std::cout << ErrorMsg << "File " << GetFileFullPath(SampleSignalFileName, i + SSigFileNameOffset, FileExtension)
                << "'s length exceeds the " << MaxLenOfSSig << " line limit.\n";
            currentFile.close();
            continue;
        }

        _sSigInfo->FileCanBeOpen[i] = true;
        currentFile.close();
    }

    std::cout << InformationMsg << "Read sample signal file done!\n";
    return 0;
}

// 选择排序，找到最大功率值并排序
int SelectSort(DSigInfo* _dSigInfo, SSigInfo* _sSigInfo, double(*sortResult)[MaxNumOfDSigFiles]) {
    int maxIndex = 0;
    maxIndex = 0;
    // 初始化排序结果
    for (int index = 0; index < MaxNumOfDSigFiles; index++) {
        sortResult[0][index] = static_cast<double>(index);
        sortResult[1][index] = _dSigInfo->PowerValue[index];
    }

    // 选择排序算法
    for (int index = 0; index < MaxNumOfDSigFiles - 1; index++) {
        int maxIndex = index;
        for (int j = index + 1; j < MaxNumOfDSigFiles; j++) {
            if (sortResult[1][j] > sortResult[1][maxIndex]) {
                maxIndex = j;
            }
        }
        // 交换
        std::swap(sortResult[0][maxIndex], sortResult[0][index]);
        std::swap(sortResult[1][maxIndex], sortResult[1][index]);
    }

    // 输出前MaxNumOfSort个最大功率值
    std::cout << InformationMsg << "Top " << MaxNumOfSort << " strength are as follows:\n";
    for (int index = 0; index < MaxNumOfSort; index++) {
        std::cout << "\t" << std::setw(2) << (index + 1) << "|File "
            << static_cast<int>(sortResult[0][index]) + DSigFileNameOffset << "\t"
            << std::fixed << std::setprecision(6) << sortResult[1][index] << "\n";
    }

    return 0;
}

// 相关性检测
int CorrelationDetection(unsigned int dSigIndex, DSigInfo* _dSigInfo, unsigned int sSigIndex, SSigInfo* _sSigInfo) {
    // 调整索引
    dSigIndex -= DSigFileNameOffset;
    sSigIndex -= SSigFileNameOffset;

    // 检查文件是否可访问
    if (!_dSigInfo->FileCanBeOpen[dSigIndex] || !_sSigInfo->FileCanBeOpen[sSigIndex]) {
        std::cout << ErrorMsg << "Deterministic Signal File or Sample Signal File is inaccessible!\n";
        return -1;
    }

    // 打开确定性信号文件
    std::string dSigFilePath = GetFileFullPath(DeterministicSignalFileName, dSigIndex + DSigFileNameOffset, FileExtension);
    std::ifstream dSigFile(dSigFilePath, std::ios::in);
    if (!dSigFile.is_open()) {
        std::cout << WarningMsg << "Failed to open " << dSigFilePath << "\n";
        return -1;
    }

    // 打开采样信号文件
    std::string sSigFilePath = GetFileFullPath(SampleSignalFileName, sSigIndex + SSigFileNameOffset, FileExtension);
    std::ifstream sSigFile(sSigFilePath, std::ios::in);
    if (!sSigFile.is_open()) {
        std::cout << WarningMsg << "Failed to open " << sSigFilePath << "\n";
        dSigFile.close();
        return -1;
    }

    // 读取文件长度
    unsigned int dSigFileLength = GetFileLength(dSigFile);
    unsigned int sSigFileLength = GetFileLength(sSigFile);

    // 生成输出文件名
    std::string outputFileName = "CorrelationResult_Data" + std::to_string(dSigIndex + DSigFileNameOffset) +
        "_PSS" + std::to_string(sSigIndex + SSigFileNameOffset) + ".csv";
    std::ofstream outputFile(outputFileName, std::ios::out);
    if (!outputFile.is_open()) {
        std::cout << WarningMsg << "Failed creating " << outputFileName << "\n";
        dSigFile.close();
        sSigFile.close();
        return -1;
    }

    // 读取确定性信号数据
    std::vector<ComplexNum> dSigData(dSigFileLength / 2);
    for (auto& num : dSigData) {
        dSigFile >> num.Re >> num.Im;
    }

    // 读取采样信号数据
    std::vector<ComplexNum> sSigData(sSigFileLength / 2);
    for (auto& num : sSigData) {
        sSigFile >> num.Re >> num.Im;
    }

    // 关闭输入文件
    dSigFile.close();
    sSigFile.close();

    double peakValue = 0.0;
    ComplexNum productValue = { 0.0, 0.0 };
    double productValueMod = 0.0;

    // 滑动窗口进行相关性检测
    for (unsigned int index = 0; index <= dSigData.size() - sSigData.size(); ++index) {
        productValue.Re = 0.0;
        productValue.Im = 0.0;

        for (size_t position = 0; position < sSigData.size(); ++position) {
            CalculateComplexNumMultiplication(dSigData[index + position], sSigData[position], productValue);
        }

        productValueMod = std::sqrt(productValue.Re * productValue.Re + productValue.Im * productValue.Im);
        outputFile << productValueMod << "\n";

        if (peakValue < productValueMod) {
            peakValue = productValueMod;
            _dSigInfo->PeakValueIndex[dSigIndex][sSigIndex] = index;
            _dSigInfo->PeakValue[dSigIndex][sSigIndex] = productValueMod;
        }
    }

    // 关闭输出文件
    outputFile.close();

    return 0;
}
