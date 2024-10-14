#ifndef SIGNAL_ANALYZE_H
#define SIGNAL_ANALYZE_H
#define MaxNumOfDSigFiles 12
#define MaxNumOfSort 6
#include <string>
#include <array>

// ��������
//constexpr unsigned int MaxNumOfDSigFiles = 12;
constexpr unsigned int MaxNumOfSSigFiles = 3;
constexpr unsigned int DSigFileNameOffset = 0;
constexpr unsigned int SSigFileNameOffset = 0;
constexpr unsigned int MaxLenOfDSig = 30000;
constexpr unsigned int MaxLenOfSSig = 4096;
//constexpr unsigned int MaxNumOfSort = 6;

// �����ռ�������֯��غ����ͱ���
//namespace SignalAnalyze {

    // �����ַ���
    extern const std::string DeterministicSignalFileName;
    extern const std::string SampleSignalFileName;
    extern const std::string FileExtension;
    extern const std::string WorkLoadEnvDir;

    extern const unsigned int StartOfDsigFiles;
    extern const unsigned int StartOfSSigFiles;

    // �ļ�������
    extern std::array<char, MaxNumOfDSigFiles> DSigFileNames;
    extern std::array<char, MaxNumOfSSigFiles> SSigFileNames;

    // �ź���������
    extern std::array<std::array<float, MaxLenOfDSig / 2>, MaxNumOfDSigFiles> DSigData;
    extern std::array<std::array<float, MaxLenOfDSig / 2>, MaxNumOfDSigFiles> DsigCorrelatedData;
    extern std::array<std::array<float, MaxLenOfSSig>, MaxNumOfSSigFiles> SSigData;

    // �����ṹ��
    struct ComplexNum {
        double Re;
        double Im;
    };

    // ȷ�����ź���Ϣ�ṹ��
    struct DSigInfo {
        std::array<unsigned int, MaxNumOfDSigFiles> FileCanBeOpen;
        std::array<double, MaxNumOfDSigFiles> PowerValue;
        std::array<std::array<double, MaxNumOfSSigFiles>, MaxNumOfDSigFiles> PeakValue;
        std::array<std::array<int, MaxNumOfSSigFiles>, MaxNumOfDSigFiles> PeakValueIndex;
    };

    // �����ź���Ϣ�ṹ��
    struct SSigInfo {
        std::array<unsigned int, MaxNumOfDSigFiles> FileCanBeOpen;
        std::array<double, MaxNumOfDSigFiles> PowerValue;
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
     * @brief ��ȡȷ�����ź��ļ��������书��
     *
     * @param dSigInfo ָ��DSigInfo�ṹ���ָ��
     * @return int �ɹ�����0��ʧ�ܷ���-1
     */
    int ReadDSigFile(DSigInfo* dSigInfo);

    /**
     * @brief ��ȡ�����ź��ļ�
     *
     * @param sSigInfo ָ��SSigInfo�ṹ���ָ��
     * @return int �ɹ�����0��ʧ�ܷ���-1
     */
    int ReadSSigFile(SSigInfo* sSigInfo);

    /**
     * @brief ѡ�������ҵ������ֵ������
     *
     * @param dSigInfo ָ��DSigInfo�ṹ���ָ��
     * @param sSigInfo ָ��SSigInfo�ṹ���ָ��
     * @param sortResult ���������飬��СΪ [2][MaxNumOfSort]
     * @return int �ɹ�����0��ʧ�ܷ���-1
     */
    //int SelectSort(DSigInfo* _dSigInfo, SSigInfo* _sSigInfo, double sortResult[][MaxNumOfDSigFiles]);
    int SelectSort(DSigInfo* _dSigInfo, SSigInfo* _sSigInfo, double(*sortResult)[MaxNumOfDSigFiles]);

    /**
     * @brief ����Լ��
     *
     * @param dSigIndex ȷ�����ź��ļ�����
     * @param dSigInfo ָ��DSigInfo�ṹ���ָ��
     * @param sSigIndex �����ź��ļ�����
     * @param sSigInfo ָ��SSigInfo�ṹ���ָ��
     * @return int �ɹ�����0��ʧ�ܷ���-1
     */
    int CorrelationDetection(unsigned int dSigIndex, DSigInfo* _dSigInfo, unsigned int sSigIndex, SSigInfo* _sSigInfo);

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
