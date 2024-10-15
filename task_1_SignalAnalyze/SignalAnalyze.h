#ifndef SIGNAL_ANALYZE_H
#define SIGNAL_ANALYZE_H

#include <string>
#include <array>
#include <fstream>
#include <vector>

//namespace SignalAnalyze {

    // ��������
    constexpr unsigned int MaxNumOfASigFiles = 12;
    constexpr unsigned int MaxNumOfSort = 6;
    constexpr unsigned int MaxNumOfDSigFiles = 3;
    constexpr unsigned int ASigFileNameOffset = 40;
    constexpr unsigned int DSigFileNameOffset = 0;
    constexpr unsigned int MaxLenOfASig = 30000;
    constexpr unsigned int MaxLenOfDSig = 4096;

    // �����ַ���
    extern const std::string ActualSignalFileName;
    extern const std::string DeterministicSignalFileName;
    extern const std::string FileExtension;
    extern const std::string WorkLoadEnvDir;

    // �ļ���ʼ����
    extern const unsigned int StartOfASigFiles;
    extern const unsigned int StartOfDSigFiles;

    // �ļ�������
    extern std::array<std::string, MaxNumOfASigFiles> ASigFileNames;
    extern std::array<std::string, MaxNumOfDSigFiles> DSigFileNames;

    // �ź���������
    extern std::array<std::array<float, MaxLenOfASig / 2>, MaxNumOfASigFiles> ASigData;
    extern std::array<std::array<float, MaxLenOfASig / 2>, MaxNumOfASigFiles> AsigCorrelatedData;
    extern std::array<std::array<float, MaxLenOfDSig>, MaxNumOfDSigFiles> DSigData;

    // �����ṹ��
    struct ComplexNum {
        double Re = 0.0;
        double Im = 0.0;
    };

    // ʵ���ź���Ϣ�ṹ��
    struct ASigInfo {
        std::array<unsigned int, MaxNumOfASigFiles> FileCanBeOpen;
        std::array<double, MaxNumOfASigFiles> PowerValue;
        std::array<std::array<double, MaxNumOfDSigFiles>, MaxNumOfASigFiles> PeakValue;
        std::array<std::array<int, MaxNumOfDSigFiles>, MaxNumOfASigFiles> PeakValueIndex;
    };

    // ȷ���ź���Ϣ�ṹ��
    struct DSigInfo {
        std::array<unsigned int, MaxNumOfASigFiles> FileCanBeOpen;
        std::array<double, MaxNumOfASigFiles> PowerValue;
    };

    // ��������

    /**
     * @brief ��ȡ�����Զ��幤��Ŀ¼���ļ�ȫ·��
     *
     * @param workLoadEnvDir ����Ŀ¼
     * @param fileClassName �ļ�������ƣ��� "data" �� "PSS"��
     * @param index �ļ�����
     * @param extension �ļ���չ������ ".txt"��
     * @return std::string �������ļ�·��
     */
    std::string GetFileFullPathSetDir(const std::string& workLoadEnvDir, const std::string& fileClassName, unsigned int index, const std::string& extension);

    /**
     * @brief ��ȡ����Ŀ¼�е��ļ�ȫ·��
     *
     * @param fileClassName �ļ�������ƣ��� "data" �� "PSS"��
     * @param index �ļ�����
     * @param extension �ļ���չ������ ".txt"��
     * @return std::string �������ļ�·��
     */
    std::string GetFileFullPath(const std::string& fileClassName, unsigned int index, const std::string& extension);

    /**
     * @brief ��ȡ�ļ����ĳ��ȣ��������ƣ�
     *
     * @param fileStream �����ļ���
     * @return unsigned int �ļ�������
     */
    unsigned int GetFileLength(std::ifstream& fileStream);

    /**
     * @brief ��ȡʵ���ź��ļ��������书��
     *
     * @param aSigInfo �ο�ʵ���ź���Ϣ�ṹ��
     * @return int �ɹ�����0��ʧ�ܷ���-1
     */
    int ReadASigFile(ASigInfo* _aSigInfo);

    /**
     * @brief ��ȡȷ���ź��ļ�
     *
     * @param dSigInfo �ο�ȷ���ź���Ϣ�ṹ��
     * @return int �ɹ�����0��ʧ�ܷ���-1
     */
    int ReadDSigFile(DSigInfo* _dSigInfo);

    /**
     * @brief ѡ�������ҵ������ֵ������
     *
     * @param aSigInfo �ο�ʵ���ź���Ϣ�ṹ��
     * @param dSigInfo �ο�ȷ���ź���Ϣ�ṹ��
     * @param sortResult ��������ά��������СΪ [2][MaxNumOfASigFiles]
     * @return int �ɹ�����0��ʧ�ܷ���-1
     */
    int SelectSort(const ASigInfo& aSigInfo, const DSigInfo& dSigInfo, std::vector<std::vector<double>>& sortResult);

    /**
     * @brief ����Լ��
     *
     * @param aSigIndex ʵ���ź��ļ�����
     * @param aSigInfo �ο�ʵ���ź���Ϣ�ṹ��
     * @param dSigIndex ȷ���ź��ļ�����
     * @param dSigInfo �ο�ȷ���ź���Ϣ�ṹ��
     * @return int �ɹ�����0��ʧ�ܷ���-1
     */
    int CorrelationDetection(unsigned int aSigIndex, ASigInfo* _aSigInfo, unsigned int dSigIndex, DSigInfo* _dSigInfo);

    /**
     * @brief ���㸴���˷�
     *
     * @param multiplicand ����������
     * @param multiplier ��������
     * @param product �������
     * @return int �ɹ�����0��ʧ�ܷ���-1
     */
    int CalculateComplexNumMultiplication(const ComplexNum& multiplicand, const ComplexNum& multiplier, ComplexNum& product);

//} // namespace SignalAnalyze

#endif // SIGNAL_ANALYZE_H
