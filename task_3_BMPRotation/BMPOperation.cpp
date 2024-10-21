#define _CRT_SECURE_NO_WARNINGS 1

#include "BMPOperation.h"
#include <vector>
#include <iostream>

const char* SrcBmpFilePath = "./�γ�����/Source.bmp"; // ԴBMP�ļ�·��
const char* DstRotatedFilePath = "./�γ�����/Rotated.bmp"; // Ŀ����תBMP�ļ�·��
const double rotationAngle = 90.0; // ��ת�Ƕ�

/**
 * @brief ��ȡBMP�ļ�ͷ
 */
bool readBmpHeader(FILE* fileStream, BITMAPFILEHEADER& fileHead, BITMAPINFOHEADER& infoHead) {
    return fread(&fileHead, sizeof(BITMAPFILEHEADER), 1, fileStream) == 1 &&
        fread(&infoHead, sizeof(BITMAPINFOHEADER), 1, fileStream) == 1;
}

/**
 * @brief д��BMP�ļ�ͷ
 */
void writeBmpHeader(FILE* fileStream, const BITMAPFILEHEADER& fileHead, const BITMAPINFOHEADER& infoHead) {
    fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fileStream);
    fwrite(&infoHead, sizeof(BITMAPINFOHEADER), 1, fileStream);
}

/**
 * @brief ��תBMPͼ��
 */
void rotateImage(const std::vector<BYTE>& srcBuffer, std::vector<BYTE>& newBuffer, int srcWidth, int srcHeight,
    int newWidth, int newHeight, double theta, int srcLineWidth, int newLineWidth, int writeWidth) {
    int srcCenterX = srcWidth / 2;
    int srcCenterY = srcHeight / 2;
    int dstCenterX = newWidth / 2;
    int dstCenterY = newHeight / 2;

    for (int hIndex = 0; hIndex < newHeight; ++hIndex) {
        for (int wIndex = 0; wIndex < newWidth; ++wIndex) {
            int index = hIndex * writeWidth + wIndex * 3; // RGB������
            double dSrcImgWIndex = (wIndex - dstCenterX) * std::cos(theta) - (hIndex - dstCenterY) * std::sin(theta) + srcCenterX;
            double dSrcImgHIndex = (wIndex - dstCenterX) * std::sin(theta) + (hIndex - dstCenterY) * std::cos(theta) + srcCenterY;

            // �߽���
            if (dSrcImgHIndex < 0 || dSrcImgWIndex < 0 || dSrcImgHIndex >= srcHeight || dSrcImgWIndex >= srcWidth) {
                newBuffer[index] = 255; // ��ɫ
                newBuffer[index + 1] = 255; // ��ɫ
                newBuffer[index + 2] = 255; // ��ɫ
                continue;
            }

            // ˫���Բ�ֵ
            int iSrcImgWIndex = static_cast<int>(dSrcImgWIndex);
            int iSrcImgHIndex = static_cast<int>(dSrcImgHIndex);
            double distance2aX = dSrcImgWIndex - iSrcImgWIndex;
            double distance2aY = dSrcImgHIndex - iSrcImgHIndex;

            // �߽���������
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

            // ������ɫֵ
            for (int color = 0; color < 3; ++color) {
                newBuffer[index + color] = (1 - distance2aX) * (1 - distance2aY) * srcBuffer[srcPA + color] +
                    (1 - distance2aX) * distance2aY * srcBuffer[srcPB + color] +
                    distance2aX * (1 - distance2aY) * srcBuffer[srcPC + color] +
                    distance2aX * distance2aY * srcBuffer[srcPD + color];
            }
        }
    }
}