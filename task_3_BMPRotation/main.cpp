#define _CRT_SECURE_NO_WARNINGS 1

#include "BMPOperation.h"
#include <iostream>
#include <vector>

int main() {
	std::cout <<InformationMsg<< "ֻ֧�� 24 λɫ�� BMP ��ʽͼƬ�ļ���" << std::endl;
    FILE* srcBmpFileStream = fopen(SrcBmpFilePath, "rb");
    if (!srcBmpFileStream) {
        std::cerr << WarningMsg << "�ļ� " << SrcBmpFilePath << " �����ڡ�" << std::endl;
        std::cerr << ErrorMsg << "��ȡ BMP �ļ����󣬳����˳���" << std::endl;
        return -1;
    }

    FILE* dstBmpFileStream = fopen(DstRotatedFilePath, "wb");
    if (!dstBmpFileStream) {
        std::cerr << ErrorMsg << "�����ļ� " << DstRotatedFilePath << " ʧ�ܡ�" << std::endl;
        std::cerr << ErrorMsg << "д�� BMP �ļ����󣬳����˳���" << std::endl;
        fclose(srcBmpFileStream);
        return -1;
    }

    BITMAPFILEHEADER fileHead;
    BITMAPINFOHEADER infoHead;

    if (!readBmpHeader(srcBmpFileStream, fileHead, infoHead)) {
        std::cerr << ErrorMsg << "��ȡ�ļ�ͷʧ�ܡ�" << std::endl;
        fclose(srcBmpFileStream);
        fclose(dstBmpFileStream);
        return -1;
    }

    double theta = rotationAngle * PI / 180;

    std::cout << "\r" << InformationMsg << "������תBMP�ļ�...";
    fflush(stdout);

    // �����ļ�ͷ
    BITMAPFILEHEADER newFileHead = fileHead;
    BITMAPINFOHEADER newInfoHead = infoHead;

    // ������ͼ��ߴ�
    int srcWidth = infoHead.biWidth;
    int srcHeight = infoHead.biHeight;
    int newWidth = std::abs(srcWidth * std::cos(theta) + srcHeight * std::sin(theta));
    int newHeight = std::abs(srcWidth * std::sin(theta) + srcHeight * std::cos(theta));

    // ����ͷ���Է�ӳ�³ߴ�
    newInfoHead.biWidth = newWidth;
    newInfoHead.biHeight = newHeight;
    int writeWidth = snap4byte(newWidth * newInfoHead.biBitCount);
    newInfoHead.biSizeImage = writeWidth * newInfoHead.biHeight;
    newFileHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + infoHead.biSizeImage;

    writeBmpHeader(dstBmpFileStream, newFileHead, newInfoHead);
    
    int srcLineWidth = snap4byte(srcWidth * infoHead.biBitCount);
    int newLineWidth = snap4byte(newInfoHead.biWidth * newInfoHead.biBitCount);

     // ʹ��vector���仺����
     std::vector<BYTE> srcBuffer(srcLineWidth * srcHeight);
     std::vector<BYTE> newBuffer(newLineWidth * newHeight, 255); // Ĭ�����Ϊ��ɫ����

     fread(srcBuffer.data(), 1, srcLineWidth * srcHeight, srcBmpFileStream);
     rotateImage(srcBuffer, newBuffer, srcWidth, srcHeight, newWidth, newHeight, theta, srcLineWidth, newLineWidth, writeWidth);

     fwrite(newBuffer.data(), 1, newLineWidth * newHeight, dstBmpFileStream);

    fclose(srcBmpFileStream);
    fclose(dstBmpFileStream);
    std::cout << "\r" << InformationMsg << "BMP�ļ���ת��ɣ�" << std::endl;
    std::cout << InformationMsg << "ͼ���ѱ��浽 " << DstRotatedFilePath << "��" << std::endl;
    return 0;
}