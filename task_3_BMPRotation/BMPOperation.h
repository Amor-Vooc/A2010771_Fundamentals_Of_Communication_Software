#ifndef BMP_OPERATION_H_
#define BMP_OPERATION_H_

#include <Windows.h>
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>

const char* const ErrorMsg = "\033[31m[ERR]\033[0m\t"; // 错误信息前缀
const char* const InformationMsg = "\033[32m[INFO]\033[0m\t"; // 信息前缀
const char* const WarningMsg = "\033[33m[WARN]\033[0m\t"; // 警告前缀

extern const char* SrcBmpFilePath; // 源BMP文件路径
extern const char* DstRotatedFilePath; // 目标旋转BMP文件路径
extern const double rotationAngle; // 旋转角度

constexpr double PI = 3.1415926;

inline size_t snap4byte(size_t input) {
    return (input + 31) / 32 * 4; // 对齐到4字节
}

/**
 * @brief 读取BMP文件头
 */
bool readBmpHeader(FILE* fileStream, BITMAPFILEHEADER& fileHead, BITMAPINFOHEADER& infoHead);

/**
 * @brief 写入BMP文件头
 */
void writeBmpHeader(FILE* fileStream, const BITMAPFILEHEADER& fileHead, const BITMAPINFOHEADER& infoHead);

/**
 * @brief 旋转BMP图像
 */
void rotateImage(const std::vector<BYTE>& srcBuffer, std::vector<BYTE>& newBuffer, int srcWidth, int srcHeight,
    int newWidth, int newHeight, double theta, int srcLineWidth, int newLineWidth, int writeWidth);

#endif // BMP_OPERATION_H_
