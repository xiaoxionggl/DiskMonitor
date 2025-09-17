/**
 * @file    main.cpp
 * @brief   CLI entry for DiskMonitor: scan a root and list top-level folder sizes.
 */

#include "FileScanner.h"

#include <iomanip>
#include <iostream>
#include <string>

using diskmon::FileScanner;
using diskmon::ScanConfig;

static void PrintHeader() {
    std::cout << "DiskMonitor - Folder Size Scanner (GB)\n"
        << "--------------------------------------\n";
}

int main(int argc, char* argv[]) {
    // 1) Ŀ��Ŀ¼���޲���ʱĬ��ɨ�� E:\

    std::string targetDir;
    if (argc < 2) {
        targetDir = "D:\\";
        std::cout << "δ�ṩɨ��Ŀ¼��Ĭ��ɨ��: " << targetDir << "\n";
    }
    else {
        targetDir = argv[1];
    }

    // 2) ɨ�����ã�Ĭ�ϣ���������
    ScanConfig cfg;
    cfg.maxDepth = 100;    // ����(  )����Ŀ¼
    cfg.minSizeGB = 0.5;  // С�� ( ) ��Ŀ¼����ʾ
    cfg.sortDescByGB = true; // �������

    try {
        PrintHeader();

        FileScanner scanner(targetDir, cfg);

        // ���Ȼص����Ե��и��Ƿ�ʽ��ӡ��UI ����Ľ��
        scanner.setProgressCallback([](int progress, const std::string& current) {
            std::cout << "\rɨ�����: " << std::setw(3) << progress << "%  "
                << "[��ǰ] " << current << std::flush;
            if (progress >= 100) std::cout << "\n";
            });

        // 3) ִ��ɨ��
        scanner.scan();

        // 4) ������
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "ɨ���������ڵ��� " << cfg.minSizeGB << " GB����\n";

        for (const auto& f : scanner.folders()) {
            std::cout << "  " << f.path << "  " << std::setw(10) << f.sizeGB << " GB\n";
        }

        std::cout << "--------------------------------------\n";
        std::cout << "�ܴ�С: " << scanner.totalSizeGB() << " GB\n";
    }
    catch (const std::exception& e) {
        std::cerr << "\n����: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

