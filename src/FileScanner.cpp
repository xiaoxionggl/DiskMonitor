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
        if (cfg_.maxDepth < 1) cfg_.maxDepth = 1; // 防御性修正
        if (cfg_.minSizeGB < 0.0) cfg_.minSizeGB = 0.0;
    }

    // ---------- public: scan ----------
    void FileScanner::scan() {
        folders_.clear();
        totalSizeGB_ = 0.0;

        // 基本校验
        const fs::path root(targetDir_);
        if (!fs::exists(root) || !fs::is_directory(root)) {
            throw std::runtime_error("目标路径不存在或不是文件夹: " + targetDir_);
        }

        // 收集第一层子目录，用于进度统计与并行扩展的分块
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
            // 遍历根目录即失败：直接抛出
            throw std::runtime_error(std::string("遍历根目录失败: ") + e.what());
        }

        const std::size_t total = subdirs.size();
        std::size_t       done = 0;

        for (const auto& dir : subdirs) {
            double szGB = 0.0;
            try {
                // 深度从 1 开始：第一层子目录
                szGB = calculateFolderSizeGB(dir.path(), /*currentDepth=*/1);
            }
            catch (const fs::filesystem_error& e) {
                // 个别目录无法访问：记录为 0，继续其他目录
                std::cerr << "[warn] 无法访问: " << dir.path().string()
                    << "，原因: " << e.what() << '\n';
                szGB = 0.0;
            }

            if (szGB >= cfg_.minSizeGB) {
                folders_.push_back(FolderInfo{ dir.path().string(), szGB });
                totalSizeGB_ += szGB;
            }

            // 进度回调（避免除零，同时限制在 100 以内）
            ++done;
            if (progressCb_ && total > 0) {
                const int progress = static_cast<int>(
                    std::min<std::size_t>(100, (done * 100) / total));
                progressCb_(progress, dir.path().string());
            }
        }

        // 排序（可配置，默认按体积降序）
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
        // 只统计从当前目录向下，受 maxDepth 限制
        // 深度定义：传入子目录 depth=1；其子目录 depth=2；... 超过即停止深入
        if (currentDepth > cfg_.maxDepth) {
            return 0.0;
        }

        std::uintmax_t totalBytes = 0;

        // 遍历当前目录（非递归），对文件累加，对子目录决定是否继续深入
        for (const auto& entry :
            fs::directory_iterator(p, fs::directory_options::skip_permission_denied)) {
            try {
                if (entry.is_regular_file()) {
                    totalBytes += entry.file_size();
                }
                else if (entry.is_directory() && !entry.is_symlink()) {
                    // 深入子目录
                    if (currentDepth < cfg_.maxDepth) {
                        const double childGB =
                            calculateFolderSizeGB(entry.path(), currentDepth + 1);
                        // 注意：childGB 是 GB，需要转回字节再累加
                        totalBytes += static_cast<std::uintmax_t>(
                            childGB * 1024.0 * 1024.0 * 1024.0);
                    }
                    else {
                        // 到达最大深度：不再深入，但如果需要，也可以在这里
                        // 选择统计“子目录一层内的文件大小”（取决于需求）。
                        // 当前实现：到达最大深度则停止深入，仅统计本层文件。
                    }
                }
            }
            catch (const fs::filesystem_error&) {
                // 单个条目访问失败：忽略并继续（鲁棒性优先）
                continue;
            }
        }

        return static_cast<double>(totalBytes) / (1024.0 * 1024.0 * 1024.0); // 字节→GB
    }

} // namespace diskmon
