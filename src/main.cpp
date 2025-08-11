#include "FileScanner.h"
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[]) {
    std::string targetDir;

    if (argc < 2) {
        targetDir = "E:\\";
        std::cout << "未提供扫描目录，默认扫描: " << targetDir << "\n";
    }
    else {
        targetDir = argv[1];
    }

    try {
        FileScanner scanner(targetDir);
        scanner.scan();

        std::cout << "扫描结果：\n";
        std::cout << std::fixed << std::setprecision(2);

        for (const auto& folder : scanner.getFolders()) {
            std::cout << folder.path << "  " << folder.sizeGB << " GB\n";
        }

        std::cout << "--------------------------------\n";
        std::cout << "总大小: " << scanner.getTotalSizeGB() << " GB\n";
    }
    catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
