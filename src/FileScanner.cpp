#include "FileScanner.h"
#include <iostream>
#include <iomanip> // std::setprecision

namespace fs = std::filesystem;

FileScanner::FileScanner(const std::string& dir) : targetDir(dir), totalSizeGB(0.0) {}

void FileScanner::scan() {
    folders.clear();
    totalSizeGB = 0.0;

    if (!fs::exists(targetDir) || !fs::is_directory(targetDir)) {
        throw std::runtime_error("目标路径不存在或不是文件夹");
    }

    // 统计子目录数量，用来计算进度
    std::vector<fs::directory_entry> subdirs;
    for (const auto& entry : fs::directory_iterator(targetDir)) {
        if (entry.is_directory()) {
            subdirs.push_back(entry);
        }
    }

    size_t total = subdirs.size();
    size_t count = 0;

    for (const auto& dir : subdirs) {
        double sizeGB = calculateFolderSizeGB(dir.path());
        folders.push_back({ dir.path().string(), sizeGB });
        totalSizeGB += sizeGB;

        // 简单进度显示
        count++;
        int progress = static_cast<int>((static_cast<double>(count) / total) * 100);
        std::cout << "\r扫描进度: " << progress << "%   " << std::flush;
    }
    std::cout << "\n";
}

double FileScanner::calculateFolderSizeGB(const fs::path& folderPath) {
    uintmax_t totalBytes = 0;
    for (auto& p : fs::recursive_directory_iterator(folderPath, fs::directory_options::skip_permission_denied)) {
        if (p.is_regular_file()) {
            totalBytes += p.file_size();
        }
    }
    return static_cast<double>(totalBytes) / (1024.0 * 1024.0 * 1024.0); // 转 GB
}

const std::vector<FolderInfo>& FileScanner::getFolders() const {
    return folders;
}

double FileScanner::getTotalSizeGB() const {
    return totalSizeGB;
}
