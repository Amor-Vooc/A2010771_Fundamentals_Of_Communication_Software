#ifndef BMP_OPERATION_H_
#define BMP_OPERATION_H_

#include <Windows.h>
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>

const char* const ErrorMsg = "\033[31m[ERR]\033[0m\t"; // ������Ϣǰ׺
const char* const InformationMsg = "\033[32m[INFO]\033[0m\t"; // ��Ϣǰ׺
const char* const WarningMsg = "\033[33m[WARN]\033[0m\t"; // ����ǰ׺

extern const char* SrcBmpFilePath; // ԴBMP�ļ�·��
extern const char* DstRotatedFilePath; // Ŀ����תBMP�ļ�·��
extern const double rotationAngle; // ��ת�Ƕ�

constexpr double PI = 3.1415926;

inline size_t snap4byte(size_t input) {
    return (input + 31) / 32 * 4; // ���뵽4�ֽ�
}

/**
 * @brief ��ȡBMP�ļ�ͷ
 */
bool readBmpHeader(FILE* fileStream, BITMAPFILEHEADER& fileHead, BITMAPINFOHEADER& infoHead);

/**
 * @brief д��BMP�ļ�ͷ
 */
void writeBmpHeader(FILE* fileStream, const BITMAPFILEHEADER& fileHead, const BITMAPINFOHEADER& infoHead);

/**
 * @brief ��תBMPͼ��
 */
void rotateImage(const std::vector<BYTE>& srcBuffer, std::vector<BYTE>& newBuffer, int srcWidth, int srcHeight,
    int newWidth, int newHeight, double theta, int srcLineWidth, int newLineWidth, int writeWidth);

#endif // BMP_OPERATION_H_
