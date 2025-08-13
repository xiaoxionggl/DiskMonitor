#pragma once
/**
 * @file    FileScanner.h
 * @brief   Folder-level size scanning with depth/threshold controls.
 * @author  张界枰
 * @date    2025-08-13
 *
 * @details
 *  提供目录体积统计能力，面向“产品化”的磁盘监测工具：
 *   - 只统计第一层子目录，或按配置递归到指定深度。
 *   - 支持最小体积阈值（忽略小目录，减少噪声）。
 *   - 通过进度回调向上层汇报扫描进度（UI 与核心逻辑解耦）。
 *   - 结果可排序、可拓展（后续可接入存储与可视化）。
 *
 *  依赖：C++17 <filesystem>
 */

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace diskmon {

    /// 单个目录的统计结果（以 GB 表示，便于直观展示）
    struct FolderInfo {
        std::string path;  ///< 目录绝对路径（UTF-8/本地编码，视平台而定）
        double      sizeGB;///< 目录总体积（GB）
    };

    /// 扫描行为配置
    struct ScanConfig {
        int    maxDepth = 1;    ///< 最大递归深度；1=仅第一层子目录
        double minSizeGB = 1.0;  ///< 最小体积阈值（GB），小于该值的结果将被过滤
        bool   sortDescByGB = true; ///< 是否按体积降序排序结果
    };

    /// 进度回调：progress [0..100]、当前处理的目录路径
    using ProgressCallback = std::function<void(int /*progress*/, const std::string& /*current*/)>;

    /**
     * @class FileScanner
     * @brief  目录体积扫描器（线程不安全；一个实例一次性使用）
     */
    class FileScanner {
    public:
        /// @brief 构造
        /// @param dir 根目录（将统计其子目录体积）
        /// @param cfg 扫描配置（深度/阈值/排序）
        explicit FileScanner(std::string dir, ScanConfig cfg = {});

        /// @brief 执行扫描（可能耗时；抛出 std::runtime_error 等异常）
        void scan();

        /// @brief 获取扫描结果（已按配置排序/过滤）
        const std::vector<FolderInfo>& folders() const noexcept { return folders_; }

        /// @brief 获取总计体积（GB）
        double totalSizeGB() const noexcept { return totalSizeGB_; }

        /// @brief 设置进度回调（可选，不设置则不输出进度）
        void setProgressCallback(ProgressCallback cb) { progressCb_ = std::move(cb); }

    private:
        // ---- helpers ----
        double calculateFolderSizeGB(const std::filesystem::path& p, int currentDepth) const;

    private:
        std::string                      targetDir_;    ///< 根目录
        ScanConfig                       cfg_;          ///< 扫描配置
        std::vector<FolderInfo>          folders_;      ///< 结果集合
        double                           totalSizeGB_ = 0.0;
        ProgressCallback                 progressCb_;   ///< 进度回调
    };

} // namespace diskmon

