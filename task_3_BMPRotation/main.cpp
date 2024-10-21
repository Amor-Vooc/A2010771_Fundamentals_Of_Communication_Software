#define _CRT_SECURE_NO_WARNINGS 1

#include "BMPOperation.h"
#include <iostream>
#include <vector>

int main() {
	std::cout <<InformationMsg<< "只支持 24 位色深 BMP 格式图片文件。" << std::endl;
    FILE* srcBmpFileStream = fopen(SrcBmpFilePath, "rb");
    if (!srcBmpFileStream) {
        std::cerr << WarningMsg << "文件 " << SrcBmpFilePath << " 不存在。" << std::endl;
        std::cerr << ErrorMsg << "读取 BMP 文件错误，程序退出。" << std::endl;
        return -1;
    }

    FILE* dstBmpFileStream = fopen(DstRotatedFilePath, "wb");
    if (!dstBmpFileStream) {
        std::cerr << ErrorMsg << "创建文件 " << DstRotatedFilePath << " 失败。" << std::endl;
        std::cerr << ErrorMsg << "写入 BMP 文件错误，程序退出。" << std::endl;
        fclose(srcBmpFileStream);
        return -1;
    }

    BITMAPFILEHEADER fileHead;
    BITMAPINFOHEADER infoHead;

    if (!readBmpHeader(srcBmpFileStream, fileHead, infoHead)) {
        std::cerr << ErrorMsg << "读取文件头失败。" << std::endl;
        fclose(srcBmpFileStream);
        fclose(dstBmpFileStream);
        return -1;
    }

    double theta = rotationAngle * PI / 180;

    std::cout << "\r" << InformationMsg << "正在旋转BMP文件...";
    fflush(stdout);

    // 复制文件头
    BITMAPFILEHEADER newFileHead = fileHead;
    BITMAPINFOHEADER newInfoHead = infoHead;

    // 计算新图像尺寸
    int srcWidth = infoHead.biWidth;
    int srcHeight = infoHead.biHeight;
    int newWidth = std::abs(srcWidth * std::cos(theta) + srcHeight * std::sin(theta));
    int newHeight = std::abs(srcWidth * std::sin(theta) + srcHeight * std::cos(theta));

    // 更新头部以反映新尺寸
    newInfoHead.biWidth = newWidth;
    newInfoHead.biHeight = newHeight;
    int writeWidth = snap4byte(newWidth * newInfoHead.biBitCount);
    newInfoHead.biSizeImage = writeWidth * newInfoHead.biHeight;
    newFileHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + infoHead.biSizeImage;

    writeBmpHeader(dstBmpFileStream, newFileHead, newInfoHead);
    
    int srcLineWidth = snap4byte(srcWidth * infoHead.biBitCount);
    int newLineWidth = snap4byte(newInfoHead.biWidth * newInfoHead.biBitCount);

     // 使用vector分配缓冲区
     std::vector<BYTE> srcBuffer(srcLineWidth * srcHeight);
     std::vector<BYTE> newBuffer(newLineWidth * newHeight, 255); // 默认填充为白色背景

     fread(srcBuffer.data(), 1, srcLineWidth * srcHeight, srcBmpFileStream);
     rotateImage(srcBuffer, newBuffer, srcWidth, srcHeight, newWidth, newHeight, theta, srcLineWidth, newLineWidth, writeWidth);

     fwrite(newBuffer.data(), 1, newLineWidth * newHeight, dstBmpFileStream);

    fclose(srcBmpFileStream);
    fclose(dstBmpFileStream);
    std::cout << "\r" << InformationMsg << "BMP文件旋转完成！" << std::endl;
    std::cout << InformationMsg << "图像已保存到 " << DstRotatedFilePath << "。" << std::endl;
    return 0;
}