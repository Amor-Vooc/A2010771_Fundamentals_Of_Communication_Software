#define _CRT_SECURE_NO_WARNINGS 1

#include "BMPOperation.h"
#include <vector>
#include <iostream>

const char* SrcBmpFilePath = "./课程资料/Source.bmp"; // 源BMP文件路径
const char* DstRotatedFilePath = "./课程资料/Rotated.bmp"; // 目标旋转BMP文件路径
const double rotationAngle = 90.0; // 旋转角度

/**
 * @brief 读取BMP文件头
 */
bool readBmpHeader(FILE* fileStream, BITMAPFILEHEADER& fileHead, BITMAPINFOHEADER& infoHead) {
    return fread(&fileHead, sizeof(BITMAPFILEHEADER), 1, fileStream) == 1 &&
        fread(&infoHead, sizeof(BITMAPINFOHEADER), 1, fileStream) == 1;
}

/**
 * @brief 写入BMP文件头
 */
void writeBmpHeader(FILE* fileStream, const BITMAPFILEHEADER& fileHead, const BITMAPINFOHEADER& infoHead) {
    fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fileStream);
    fwrite(&infoHead, sizeof(BITMAPINFOHEADER), 1, fileStream);
}

/**
 * @brief 旋转BMP图像
 */
void rotateImage(const std::vector<BYTE>& srcBuffer, std::vector<BYTE>& newBuffer, int srcWidth, int srcHeight,
    int newWidth, int newHeight, double theta, int srcLineWidth, int newLineWidth, int writeWidth) {
    int srcCenterX = srcWidth / 2;
    int srcCenterY = srcHeight / 2;
    int dstCenterX = newWidth / 2;
    int dstCenterY = newHeight / 2;

    for (int hIndex = 0; hIndex < newHeight; ++hIndex) {
        for (int wIndex = 0; wIndex < newWidth; ++wIndex) {
            int index = hIndex * writeWidth + wIndex * 3; // RGB的索引
            double dSrcImgWIndex = (wIndex - dstCenterX) * std::cos(theta) - (hIndex - dstCenterY) * std::sin(theta) + srcCenterX;
            double dSrcImgHIndex = (wIndex - dstCenterX) * std::sin(theta) + (hIndex - dstCenterY) * std::cos(theta) + srcCenterY;

            // 边界检查
            if (dSrcImgHIndex < 0 || dSrcImgWIndex < 0 || dSrcImgHIndex >= srcHeight || dSrcImgWIndex >= srcWidth) {
                newBuffer[index] = 255; // 蓝色
                newBuffer[index + 1] = 255; // 绿色
                newBuffer[index + 2] = 255; // 红色
                continue;
            }

            // 双线性插值
            int iSrcImgWIndex = static_cast<int>(dSrcImgWIndex);
            int iSrcImgHIndex = static_cast<int>(dSrcImgHIndex);
            double distance2aX = dSrcImgWIndex - iSrcImgWIndex;
            double distance2aY = dSrcImgHIndex - iSrcImgHIndex;

            // 边界条件处理
            int srcPA = iSrcImgHIndex * srcLineWidth + iSrcImgWIndex * 3;
            int srcPB = srcPA + 3;
            int srcPC = (iSrcImgHIndex + 1) * srcLineWidth + iSrcImgWIndex * 3;
            int srcPD = srcPC + 3;

            if (iSrcImgWIndex >= srcWidth - 1) {
                srcPB = srcPA;
                srcPD = srcPC;
            }
            if (iSrcImgHIndex >= srcHeight - 1) {
                srcPC = srcPA;
                srcPD = srcPB;
            }

            // 计算颜色值
            for (int color = 0; color < 3; ++color) {
                newBuffer[index + color] = (1 - distance2aX) * (1 - distance2aY) * srcBuffer[srcPA + color] +
                    (1 - distance2aX) * distance2aY * srcBuffer[srcPB + color] +
                    distance2aX * (1 - distance2aY) * srcBuffer[srcPC + color] +
                    distance2aX * distance2aY * srcBuffer[srcPD + color];
            }
        }
    }
}