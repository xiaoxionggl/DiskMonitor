#include "FileScanner.h"
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[]) {
    std::string targetDir;

    if (argc < 2) {
        targetDir = "E:\\";
        std::cout << "δ�ṩɨ��Ŀ¼��Ĭ��ɨ��: " << targetDir << "\n";
    }
    else {
        targetDir = argv[1];
    }

    try {
        FileScanner scanner(targetDir);
        scanner.scan();

        std::cout << "ɨ������\n";
        std::cout << std::fixed << std::setprecision(2);

        for (const auto& folder : scanner.getFolders()) {
            std::cout << folder.path << "  " << folder.sizeGB << " GB\n";
        }

        std::cout << "--------------------------------\n";
        std::cout << "�ܴ�С: " << scanner.getTotalSizeGB() << " GB\n";
    }
    catch (const std::exception& e) {
        std::cerr << "����: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
