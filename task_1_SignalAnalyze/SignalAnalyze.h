#ifndef SIGNAL_ANALYZE_H
#define SIGNAL_ANALYZE_H

#include <string>
#include <array>
#include <fstream>
#include <vector>

//namespace SignalAnalyze {

    // 常量定义
    constexpr unsigned int MaxNumOfASigFiles = 12;
    constexpr unsigned int MaxNumOfSort = 6;
    constexpr unsigned int MaxNumOfDSigFiles = 3;
    constexpr unsigned int ASigFileNameOffset = 40;
    constexpr unsigned int DSigFileNameOffset = 0;
    constexpr unsigned int MaxLenOfASig = 30000;
    constexpr unsigned int MaxLenOfDSig = 4096;

    // 常量字符串
    extern const std::string ActualSignalFileName;
    extern const std::string DeterministicSignalFileName;
    extern const std::string FileExtension;
    extern const std::string WorkLoadEnvDir;

    // 文件起始索引
    extern const unsigned int StartOfASigFiles;
    extern const unsigned int StartOfDSigFiles;

    // 文件名数组
    extern std::array<std::string, MaxNumOfASigFiles> ASigFileNames;
    extern std::array<std::string, MaxNumOfDSigFiles> DSigFileNames;

    // 信号数据数组
    extern std::array<std::array<float, MaxLenOfASig / 2>, MaxNumOfASigFiles> ASigData;
    extern std::array<std::array<float, MaxLenOfASig / 2>, MaxNumOfASigFiles> AsigCorrelatedData;
    extern std::array<std::array<float, MaxLenOfDSig>, MaxNumOfDSigFiles> DSigData;

    // 复数结构体
    struct ComplexNum {
        double Re = 0.0;
        double Im = 0.0;
    };

    // 实际信号信息结构体
    struct ASigInfo {
        std::array<unsigned int, MaxNumOfASigFiles> FileCanBeOpen;
        std::array<double, MaxNumOfASigFiles> PowerValue;
        std::array<std::array<double, MaxNumOfDSigFiles>, MaxNumOfASigFiles> PeakValue;
        std::array<std::array<int, MaxNumOfDSigFiles>, MaxNumOfASigFiles> PeakValueIndex;
    };

    // 确定信号信息结构体
    struct DSigInfo {
        std::array<unsigned int, MaxNumOfASigFiles> FileCanBeOpen;
        std::array<double, MaxNumOfASigFiles> PowerValue;
    };

    // 函数声明

    /**
     * @brief 获取带有自定义工作目录的文件全路径
     *
     * @param workLoadEnvDir 工作目录
     * @param fileClassName 文件类别名称（如 "data" 或 "PSS"）
     * @param index 文件索引
     * @param extension 文件扩展名（如 ".txt"）
     * @return std::string 完整的文件路径
     */
    std::string GetFileFullPathSetDir(const std::string& workLoadEnvDir, const std::string& fileClassName, unsigned int index, const std::string& extension);

    /**
     * @brief 获取工作目录中的文件全路径
     *
     * @param fileClassName 文件类别名称（如 "data" 或 "PSS"）
     * @param index 文件索引
     * @param extension 文件扩展名（如 ".txt"）
     * @return std::string 完整的文件路径
     */
    std::string GetFileFullPath(const std::string& fileClassName, unsigned int index, const std::string& extension);

    /**
     * @brief 获取文件流的长度（以行数计）
     *
     * @param fileStream 输入文件流
     * @return unsigned int 文件的行数
     */
    unsigned int GetFileLength(std::ifstream& fileStream);

    /**
     * @brief 读取实际信号文件并计算其功率
     *
     * @param aSigInfo 参考实际信号信息结构体
     * @return int 成功返回0，失败返回-1
     */
    int ReadASigFile(ASigInfo* _aSigInfo);

    /**
     * @brief 读取确定信号文件
     *
     * @param dSigInfo 参考确定信号信息结构体
     * @return int 成功返回0，失败返回-1
     */
    int ReadDSigFile(DSigInfo* _dSigInfo);

    /**
     * @brief 选择排序，找到最大功率值并排序
     *
     * @param aSigInfo 参考实际信号信息结构体
     * @param dSigInfo 参考确定信号信息结构体
     * @param sortResult 排序结果二维向量，大小为 [2][MaxNumOfASigFiles]
     * @return int 成功返回0，失败返回-1
     */
    int SelectSort(const ASigInfo& aSigInfo, const DSigInfo& dSigInfo, std::vector<std::vector<double>>& sortResult);

    /**
     * @brief 相关性检测
     *
     * @param aSigIndex 实际信号文件索引
     * @param aSigInfo 参考实际信号信息结构体
     * @param dSigIndex 确定信号文件索引
     * @param dSigInfo 参考确定信号信息结构体
     * @return int 成功返回0，失败返回-1
     */
    int CorrelationDetection(unsigned int aSigIndex, ASigInfo* _aSigInfo, unsigned int dSigIndex, DSigInfo* _dSigInfo);

    /**
     * @brief 计算复数乘法
     *
     * @param multiplicand 被乘数复数
     * @param multiplier 乘数复数
     * @param product 结果复数
     * @return int 成功返回0，失败返回-1
     */
    int CalculateComplexNumMultiplication(const ComplexNum& multiplicand, const ComplexNum& multiplier, ComplexNum& product);

//} // namespace SignalAnalyze

#endif // SIGNAL_ANALYZE_H
