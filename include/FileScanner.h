#pragma once
#include <string>
#include <vector>
#include <filesystem>

struct FolderInfo {
    std::string path;
    double sizeGB; // ”√ GB ¥Ê¥¢
};

class FileScanner {
public:
    FileScanner(const std::string& dir);
    void scan();
    const std::vector<FolderInfo>& getFolders() const;
    double getTotalSizeGB() const;

private:
    std::string targetDir;
    std::vector<FolderInfo> folders;
    double totalSizeGB;

    double calculateFolderSizeGB(const std::filesystem::path& folderPath);
};
