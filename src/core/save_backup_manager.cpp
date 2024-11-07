// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QTimer>
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

bool SaveBackupManager::createBackupDirectory(const QString& path) {
    QDir dir(path);
    if (!dir.exists()) {
        return dir.mkpath(".");
    }
    return true;
}

void SaveBackupManager::copyDirectory(const QString& src, const QString& dst) {
    QDir srcDir(src);
    if (!srcDir.exists())
        return;

    QDir dstDir(dst);
    if (!dstDir.exists()) {
        dstDir.mkpath(".");
    }

    QStringList files = srcDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString& file : files) {
        QString srcPath = srcDir.filePath(file);
        QString dstPath = dstDir.filePath(file);

        if (QFileInfo(srcPath).isDir()) {
            copyDirectory(srcPath, dstPath);
        } else {
            QFile::copy(srcPath, dstPath);
        }
    }
}

void SaveBackupManager::performBackup() {
    if (!backup_enabled || backup_directory.isEmpty()) {
        return;
    }

    // Get current save directory path
    QString save_dir = "ShadPS4/user/savedata"; // Actual save directory
    
    if (!QDir(save_dir).exists()) {
        return;
    }

    // Create backup directory with timestamp
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    QString backup_path = backup_directory + "/backup_" + timestamp;

    if (createBackupDirectory(backup_path)) {
        copyDirectory(save_dir, backup_path);
    }
}

} // namespace Core
