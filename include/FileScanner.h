#pragma once
/**
 * @file    FileScanner.h
 * @brief   Folder-level size scanning with depth/threshold controls.
 * @author  �Ž���
 * @date    2025-08-13
 *
 * @details
 *  �ṩĿ¼���ͳ�����������򡰲�Ʒ�����Ĵ��̼�⹤�ߣ�
 *   - ֻͳ�Ƶ�һ����Ŀ¼�������õݹ鵽ָ����ȡ�
 *   - ֧����С�����ֵ������СĿ¼��������������
 *   - ͨ�����Ȼص����ϲ�㱨ɨ����ȣ�UI ������߼������
 *   - ��������򡢿���չ�������ɽ���洢����ӻ�����
 *
 *  ������C++17 <filesystem>
 */

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace diskmon {

    /// ����Ŀ¼��ͳ�ƽ������ GB ��ʾ������ֱ��չʾ��
    struct FolderInfo {
        std::string path;  ///< Ŀ¼����·����UTF-8/���ر��룬��ƽ̨������
        double      sizeGB;///< Ŀ¼�������GB��
    };

    /// ɨ����Ϊ����
    struct ScanConfig {
        int    maxDepth = 1;    ///< ���ݹ���ȣ�1=����һ����Ŀ¼
        double minSizeGB = 1.0;  ///< ��С�����ֵ��GB����С�ڸ�ֵ�Ľ����������
        bool   sortDescByGB = true; ///< �Ƿ��������������
    };

    /// ���Ȼص���progress [0..100]����ǰ�����Ŀ¼·��
    using ProgressCallback = std::function<void(int /*progress*/, const std::string& /*current*/)>;

    /**
     * @class FileScanner
     * @brief  Ŀ¼���ɨ�������̲߳���ȫ��һ��ʵ��һ����ʹ�ã�
     */
    class FileScanner {
    public:
        /// @brief ����
        /// @param dir ��Ŀ¼����ͳ������Ŀ¼�����
        /// @param cfg ɨ�����ã����/��ֵ/����
        explicit FileScanner(std::string dir, ScanConfig cfg = {});

        /// @brief ִ��ɨ�裨���ܺ�ʱ���׳� std::runtime_error ���쳣��
        void scan();

        /// @brief ��ȡɨ�������Ѱ���������/���ˣ�
        const std::vector<FolderInfo>& folders() const noexcept { return folders_; }

        /// @brief ��ȡ�ܼ������GB��
        double totalSizeGB() const noexcept { return totalSizeGB_; }

        /// @brief ���ý��Ȼص�����ѡ����������������ȣ�
        void setProgressCallback(ProgressCallback cb) { progressCb_ = std::move(cb); }

    private:
        // ---- helpers ----
        double calculateFolderSizeGB(const std::filesystem::path& p, int currentDepth) const;

    private:
        std::string                      targetDir_;    ///< ��Ŀ¼
        ScanConfig                       cfg_;          ///< ɨ������
        std::vector<FolderInfo>          folders_;      ///< �������
        double                           totalSizeGB_ = 0.0;
        ProgressCallback                 progressCb_;   ///< ���Ȼص�
    };

} // namespace diskmon

