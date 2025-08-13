/**
 * @file    FileScanner.cpp
 * @brief   Implementation of folder-level size scanning.
 */

#include "FileScanner.h"

#include <algorithm>
#include <iomanip>
#include <iostream>

namespace fs = std::filesystem;
namespace diskmon {

    // ---------- ctor ----------
    FileScanner::FileScanner(std::string dir, ScanConfig cfg)
        : targetDir_(std::move(dir)), cfg_(cfg) {
        if (cfg_.maxDepth < 1) cfg_.maxDepth = 1; // ����������
        if (cfg_.minSizeGB < 0.0) cfg_.minSizeGB = 0.0;
    }

    // ---------- public: scan ----------
    void FileScanner::scan() {
        folders_.clear();
        totalSizeGB_ = 0.0;

        // ����У��
        const fs::path root(targetDir_);
        if (!fs::exists(root) || !fs::is_directory(root)) {
            throw std::runtime_error("Ŀ��·�������ڻ����ļ���: " + targetDir_);
        }

        // �ռ���һ����Ŀ¼�����ڽ���ͳ���벢����չ�ķֿ�
        std::vector<fs::directory_entry> subdirs;
        try {
            for (const auto& entry : fs::directory_iterator(
                root, fs::directory_options::skip_permission_denied)) {
                if (entry.is_directory() && !entry.is_symlink()) {
                    subdirs.emplace_back(entry);
                }
            }
        }
        catch (const fs::filesystem_error& e) {
            // ������Ŀ¼��ʧ�ܣ�ֱ���׳�
            throw std::runtime_error(std::string("������Ŀ¼ʧ��: ") + e.what());
        }

        const std::size_t total = subdirs.size();
        std::size_t       done = 0;

        for (const auto& dir : subdirs) {
            double szGB = 0.0;
            try {
                // ��ȴ� 1 ��ʼ����һ����Ŀ¼
                szGB = calculateFolderSizeGB(dir.path(), /*currentDepth=*/1);
            }
            catch (const fs::filesystem_error& e) {
                // ����Ŀ¼�޷����ʣ���¼Ϊ 0����������Ŀ¼
                std::cerr << "[warn] �޷�����: " << dir.path().string()
                    << "��ԭ��: " << e.what() << '\n';
                szGB = 0.0;
            }

            if (szGB >= cfg_.minSizeGB) {
                folders_.push_back(FolderInfo{ dir.path().string(), szGB });
                totalSizeGB_ += szGB;
            }

            // ���Ȼص���������㣬ͬʱ������ 100 ���ڣ�
            ++done;
            if (progressCb_ && total > 0) {
                const int progress = static_cast<int>(
                    std::min<std::size_t>(100, (done * 100) / total));
                progressCb_(progress, dir.path().string());
            }
        }

        // ���򣨿����ã�Ĭ�ϰ��������
        if (cfg_.sortDescByGB) {
            std::sort(folders_.begin(), folders_.end(),
                [](const FolderInfo& a, const FolderInfo& b) { return a.sizeGB > b.sizeGB; });
        }
        else {
            std::sort(folders_.begin(), folders_.end(),
                [](const FolderInfo& a, const FolderInfo& b) { return a.sizeGB < b.sizeGB; });
        }
    }

    // ---------- private: calculateFolderSizeGB ----------
    double FileScanner::calculateFolderSizeGB(const fs::path& p, int currentDepth) const {
        // ֻͳ�ƴӵ�ǰĿ¼���£��� maxDepth ����
        // ��ȶ��壺������Ŀ¼ depth=1������Ŀ¼ depth=2��... ������ֹͣ����
        if (currentDepth > cfg_.maxDepth) {
            return 0.0;
        }

        std::uintmax_t totalBytes = 0;

        // ������ǰĿ¼���ǵݹ飩�����ļ��ۼӣ�����Ŀ¼�����Ƿ��������
        for (const auto& entry :
            fs::directory_iterator(p, fs::directory_options::skip_permission_denied)) {
            try {
                if (entry.is_regular_file()) {
                    totalBytes += entry.file_size();
                }
                else if (entry.is_directory() && !entry.is_symlink()) {
                    // ������Ŀ¼
                    if (currentDepth < cfg_.maxDepth) {
                        const double childGB =
                            calculateFolderSizeGB(entry.path(), currentDepth + 1);
                        // ע�⣺childGB �� GB����Ҫת���ֽ����ۼ�
                        totalBytes += static_cast<std::uintmax_t>(
                            childGB * 1024.0 * 1024.0 * 1024.0);
                    }
                    else {
                        // ���������ȣ��������룬�������Ҫ��Ҳ����������
                        // ѡ��ͳ�ơ���Ŀ¼һ���ڵ��ļ���С����ȡ�������󣩡�
                        // ��ǰʵ�֣�������������ֹͣ���룬��ͳ�Ʊ����ļ���
                    }
                }
            }
            catch (const fs::filesystem_error&) {
                // ������Ŀ����ʧ�ܣ����Բ�������³�������ȣ�
                continue;
            }
        }

        return static_cast<double>(totalBytes) / (1024.0 * 1024.0 * 1024.0); // �ֽڡ�GB
    }

} // namespace diskmon
