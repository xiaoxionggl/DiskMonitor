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
    // 1) 目标目录：无参数时默认扫描 E:\

    std::string targetDir;
    if (argc < 2) {
        targetDir = "D:\\";
        std::cout << "未提供扫描目录，默认扫描: " << targetDir << "\n";
    }
    else {
        targetDir = argv[1];
    }

    // 2) 扫描配置：默认（降序排序）
    ScanConfig cfg;
    cfg.maxDepth = 100;    // 仅第(  )层子目录
    cfg.minSizeGB = 0.5;  // 小于 ( ) 的目录不显示
    cfg.sortDescByGB = true; // 体积降序

    try {
        PrintHeader();

        FileScanner scanner(targetDir, cfg);

        // 进度回调：以单行覆盖方式打印（UI 与核心解耦）
        scanner.setProgressCallback([](int progress, const std::string& current) {
            std::cout << "\r扫描进度: " << std::setw(3) << progress << "%  "
                << "[当前] " << current << std::flush;
            if (progress >= 100) std::cout << "\n";
            });

        // 3) 执行扫描
        scanner.scan();

        // 4) 输出结果
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "扫描结果（大于等于 " << cfg.minSizeGB << " GB）：\n";

        for (const auto& f : scanner.folders()) {
            std::cout << "  " << f.path << "  " << std::setw(10) << f.sizeGB << " GB\n";
        }

        std::cout << "--------------------------------------\n";
        std::cout << "总大小: " << scanner.totalSizeGB() << " GB\n";
    }
    catch (const std::exception& e) {
        std::cerr << "\n错误: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

