#ifndef SIGNAL_ANALYZE_H
#define SIGNAL_ANALYZE_H
#define MaxNumOfASigFiles 12
#define MaxNumOfSort 6
#include <string>
#include <array>

// ��������
//constexpr unsigned int MaxNumOfASigFiles = 12;
constexpr unsigned int MaxNumOfDSigFiles = 3;
constexpr unsigned int ASigFileNameOffset = 40;
constexpr unsigned int DSigFileNameOffset = 0;
constexpr unsigned int MaxLenOfASig = 30000;
constexpr unsigned int MaxLenOfDSig = 4096;
//constexpr unsigned int MaxNumOfSort = 6;

// �����ռ�������֯��غ����ͱ���
//namespace SignalAnalyze {

    // �����ַ���
    extern const std::string ActualSignalFileName;
    extern const std::string DeterministicSignalFileName;
    extern const std::string FileExtension;
    extern const std::string WorkLoadEnvDir;

    extern const unsigned int StartOfAsigFiles;
    extern const unsigned int StartOfDSigFiles;

    // �ļ�������
    extern std::array<char, MaxNumOfASigFiles> ASigFileNames;
    extern std::array<char, MaxNumOfDSigFiles> DSigFileNames;

    // �ź���������
    extern std::array<std::array<float, MaxLenOfASig / 2>, MaxNumOfASigFiles> ASigData;
    extern std::array<std::array<float, MaxLenOfASig / 2>, MaxNumOfASigFiles> AsigCorrelatedData;
    extern std::array<std::array<float, MaxLenOfDSig>, MaxNumOfDSigFiles> DSigData;

    // �����ṹ��
    struct ComplexNum {
        double Re;
        double Im;
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
     * @param aSigInfo ָ��ASigInfo�ṹ���ָ��
     * @return int �ɹ�����0��ʧ�ܷ���-1
     */
    int ReadASigFile(ASigInfo* aSigInfo);

    /**
     * @brief ��ȡȷ���ź��ļ�
     *
     * @param dSigInfo ָ��DSigInfo�ṹ���ָ��
     * @return int �ɹ�����0��ʧ�ܷ���-1
     */
    int ReadDSigFile(DSigInfo* dSigInfo);

    /**
     * @brief ѡ�������ҵ������ֵ������
     *
     * @param aSigInfo ָ��ASigInfo�ṹ���ָ��
     * @param dSigInfo ָ��DSigInfo�ṹ���ָ��
     * @param sortResult ���������飬��СΪ [2][MaxNumOfSort]
     * @return int �ɹ�����0��ʧ�ܷ���-1
     */
    int SelectSort(ASigInfo* _aSigInfo, DSigInfo* _dSigInfo, double(*sortResult)[MaxNumOfASigFiles]);

    /**
     * @brief ����Լ��
     *
     * @param aSigIndex ʵ���ź��ļ�����
     * @param aSigInfo ָ��ASigInfo�ṹ���ָ��
     * @param dSigIndex ȷ���ź��ļ�����
     * @param dSigInfo ָ��DSigInfo�ṹ���ָ��
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
