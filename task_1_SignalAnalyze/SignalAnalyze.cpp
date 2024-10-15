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
const std::string ActualSignalFileName = "data";
const std::string DeterministicSignalFileName = "PSS";
const std::string FileExtension = ".txt";
const std::string WorkLoadEnvDir = "D:/个人文件/作业/第三学期_大二上/通信软件基础/Fundamentals_Of_Communication_Software/task_1_SignalAnalyze/课程资料/第一次实验数据/"; //信号文件路径

const unsigned int StartOfAsigFiles = 40;
const unsigned int StartOfDSigFiles = 0;

extern ASigInfo aSigInfo;
extern DSigInfo dSigInfo;

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

// 读取实际信号文件并计算其功率
int ReadASigFile(ASigInfo* _aSigInfo) {
    ComplexNum currentComplexNum;
    std::ifstream currentFile;
    unsigned int currentFileLength;

    std::cout << InformationMsg << "开始读取实际信号文件.\n";

    for (int i = 0; i < MaxNumOfASigFiles; i++) {
        _aSigInfo->FileCanBeOpen[i] = 0;

        std::string filePath = GetFileFullPath(ActualSignalFileName, i + ASigFileNameOffset, FileExtension);
        currentFile.open(filePath, std::ios::in);
        if (!currentFile.is_open()) {
            std::cout << WarningMsg << "文件 " << filePath << " 不存在.\n";
            continue;
        }

        currentFileLength = GetFileLength(currentFile);
        if (currentFileLength > MaxLenOfASig) {
            std::cout << ErrorMsg << "文件 " << GetFileFullPath(ActualSignalFileName, i + ASigFileNameOffset, FileExtension)
                << "的长度超过了 " << MaxLenOfASig << " 行数限制.\n";
            currentFile.close();
            continue;
        }

        _aSigInfo->PowerValue[i] = 0.0;
        currentComplexNum.Re = 0.0;
        currentComplexNum.Im = 0.0;

        // 计算功率值
        while (currentFile >> currentComplexNum.Re >> currentComplexNum.Im) {
            _aSigInfo->PowerValue[i] += std::sqrt(currentComplexNum.Re * currentComplexNum.Re + currentComplexNum.Im * currentComplexNum.Im);
        }

        std::cout << InformationMsg << "文件 " << filePath << " 的功率值为 " << std::fixed << std::setprecision(6)
            << _aSigInfo->PowerValue[i] << ".\n";
        _aSigInfo->FileCanBeOpen[i] = true;
        currentFile.close();
    }

    std::cout << InformationMsg << "读取实际信号文件完成!\n";
    return 0;
}

// 读取确定信号文件
int ReadDSigFile(DSigInfo* _dSigInfo) {
    std::ifstream currentFile;
    unsigned int currentFileLength;

    std::cout << InformationMsg << "开始读取确定信号文件.\n";

    for (int i = 0; i < MaxNumOfDSigFiles; i++) {
        _dSigInfo->FileCanBeOpen[i] = false;

        std::string filePath = GetFileFullPath(DeterministicSignalFileName, i + DSigFileNameOffset, FileExtension);
        currentFile.open(filePath, std::ios::in);
        if (!currentFile.is_open()) {
            std::cout << WarningMsg << "文件 " << filePath << " 不存在.\n";
            continue;
        }

        currentFileLength = GetFileLength(currentFile);
        if (currentFileLength > MaxLenOfDSig) {
            std::cout << ErrorMsg << "文件 " << GetFileFullPath(DeterministicSignalFileName, i + DSigFileNameOffset, FileExtension)
                << "的长度超过了 " << MaxLenOfDSig << " 行数限制.\n";
            currentFile.close();
            continue;
        }

        _dSigInfo->FileCanBeOpen[i] = true;
        currentFile.close();
    }

    std::cout << InformationMsg << "读取确定信号文件完成!\n";
    return 0;
}

// 选择排序，找到最大功率值并排序
int SelectSort(ASigInfo* _aSigInfo, DSigInfo* _dSigInfo, double(*sortResult)[MaxNumOfASigFiles]) {
    int maxIndex = 0;
    maxIndex = 0;
    // 初始化排序结果
    for (int index = 0; index < MaxNumOfASigFiles; index++) {
        sortResult[0][index] = static_cast<double>(index);
        sortResult[1][index] = _aSigInfo->PowerValue[index];
    }

    // 选择排序算法
    for (int index = 0; index < MaxNumOfASigFiles - 1; index++) {
        int maxIndex = index;
        for (int j = index + 1; j < MaxNumOfASigFiles; j++) {
            if (sortResult[1][j] > sortResult[1][maxIndex]) {
                maxIndex = j;
            }
        }
        // 交换
        std::swap(sortResult[0][maxIndex], sortResult[0][index]);
        std::swap(sortResult[1][maxIndex], sortResult[1][index]);
    }

    // 输出前MaxNumOfSort个最大功率值
    std::cout << InformationMsg << "最大的 " << MaxNumOfSort << " 个功率值如下:\n";
    for (int index = 0; index < MaxNumOfSort; index++) {
        std::cout << "\t" << std::setw(2) << (index + 1) << "|文件 "
            << static_cast<int>(sortResult[0][index]) + ASigFileNameOffset << "\t"
            << std::fixed << std::setprecision(6) << sortResult[1][index] << "\n";
    }

    return 0;
}

// 相关性检测
int CorrelationDetection(unsigned int aSigIndex, ASigInfo* _aSigInfo, unsigned int dSigIndex, DSigInfo* _dSigInfo) {
    // 调整索引
    aSigIndex -= ASigFileNameOffset;
    dSigIndex -= DSigFileNameOffset;

    // 检查文件是否可访问
    if (!_aSigInfo->FileCanBeOpen[aSigIndex] || !_dSigInfo->FileCanBeOpen[dSigIndex]) {
        std::cout << ErrorMsg << "实际信号文件或确定信号文件不可访问!\n";
        return -1;
    }

    // 打开实际信号文件
    std::string aSigFilePath = GetFileFullPath(ActualSignalFileName, aSigIndex + ASigFileNameOffset, FileExtension);
    std::ifstream aSigFile(aSigFilePath, std::ios::in);
    if (!aSigFile.is_open()) {
        std::cout << WarningMsg << "无法打开 " << aSigFilePath << std::endl;
        return -1;
    }

    // 打开确定信号文件
    std::string dSigFilePath = GetFileFullPath(DeterministicSignalFileName, dSigIndex + DSigFileNameOffset, FileExtension);
    std::ifstream dSigFile(dSigFilePath, std::ios::in);
    if (!dSigFile.is_open()) {
        std::cout << WarningMsg << "无法打开 " << dSigFilePath << std::endl;
        aSigFile.close();
        return -1;
    }

    // 读取文件长度
    unsigned int aSigFileLength = GetFileLength(aSigFile);
    unsigned int dSigFileLength = GetFileLength(dSigFile);

    // 生成输出文件名
    std::string outputFileName = "CorrelationResult_Data" + std::to_string(aSigIndex + ASigFileNameOffset) +
        "_PSS" + std::to_string(dSigIndex + DSigFileNameOffset) + ".csv";
    std::ofstream outputFile(outputFileName, std::ios::out);
    if (!outputFile.is_open()) {
        std::cout << WarningMsg << "创建失败 " << outputFileName << std::endl;
        aSigFile.close();
        dSigFile.close();
        return -1;
    }

    // 读取实际信号数据
    std::vector<ComplexNum> aSigData(aSigFileLength / 2);
    for (auto& num : aSigData) {
        aSigFile >> num.Re >> num.Im;
    }

    // 读取确定信号数据
    std::vector<ComplexNum> dSigData(dSigFileLength / 2);
    for (auto& num : dSigData) {
        dSigFile >> num.Re >> num.Im;
    }

    // 关闭输入文件
    aSigFile.close();
    dSigFile.close();

    double peakValue = 0.0;
    ComplexNum productValue = { 0.0, 0.0 };
    double productValueMod = 0.0;

    // 滑动窗口进行相关性检测
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

    // 关闭输出文件
    outputFile.close();

    return 0;
}
