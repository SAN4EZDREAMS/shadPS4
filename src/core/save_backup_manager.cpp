// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QDir>
#include "core/save_backup_manager.h"

namespace Core {

SaveBackupManager::SaveBackupManager(QObject* parent) : QObject(parent) {
    backup_timer = new QTimer(this);
    connect(backup_timer, &QTimer::timeout, this, &SaveBackupManager::performBackup);
}

SaveBackupManager::~SaveBackupManager() {
    if (backup_timer->isActive()) {
        backup_timer->stop();
    }
}

void SaveBackupManager::setBackupEnabled(bool enabled) {
    backup_enabled = enabled;
    if (enabled && !backup_directory.isEmpty()) {
        backup_timer->start(backup_interval * 60 * 1000); // Convert minutes to milliseconds
    } else {
        backup_timer->stop();
    }
}

void SaveBackupManager::setBackupInterval(int minutes) {
    backup_interval = minutes;
    if (backup_timer->isActive()) {
        backup_timer->start(backup_interval * 60 * 1000);
    }
}

void SaveBackupManager::setBackupDirectory(const QString& path) {
    backup_directory = path;
}

bool SaveBackupManager::createBackupDirectory(const std::filesystem::path& path) {
    try {
        if (!std::filesystem::exists(path)) {
            return std::filesystem::create_directories(path);
        }
        return true;
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

void SaveBackupManager::copyDirectory(const std::filesystem::path& src, const std::filesystem::path& dst) {
    try {
        if (!std::filesystem::exists(src)) {
            return;
        }

        if (!std::filesystem::exists(dst)) {
            std::filesystem::create_directories(dst);
        }

        for (const auto& entry : std::filesystem::directory_iterator(src)) {
            const auto& path = entry.path();
            const auto destination = dst / path.filename();
            
            if (std::filesystem::is_directory(path)) {
                copyDirectory(path, destination);
            } else {
                std::filesystem::copy_file(path, destination, 
                    std::filesystem::copy_options::overwrite_existing);
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        // Handle error
    }
}

void SaveBackupManager::performBackup() {
    if (!backup_enabled || backup_directory.isEmpty()) {
        return;
    }

    // Get current save directory path
    const auto save_dir = std::filesystem::path("ShadPS4/user/savedata"); // Actual save directory
    
    if (!std::filesystem::exists(save_dir)) {
        return;
    }

    // Create backup directory with timestamp
    const auto timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    const auto backup_path = std::filesystem::path(backup_directory.toStdString()) / 
                            ("backup_" + timestamp.toStdString());

    if (createBackupDirectory(backup_path)) {
        copyDirectory(save_dir, backup_path);
    }
}

} // namespace Core
