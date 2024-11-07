// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <QString>
#include <QTimer>
#include <QDateTime>
#include <filesystem>

namespace Core {

class SaveBackupManager {
public:
    void Initialize();
    void CreateBackup(const std::string& save_id);
    void RestoreBackup(const std::string& backup_id);
    std::vector<BackupInfo> ListBackups(const std::string& save_id);
    void PruneOldBackups();
    void ScheduleAutoBackup();
    
private:
    std::string GetBackupPath(const std::string& save_id);
    void LoadConfig();
    void SaveConfig();
    
    Config& config;
    std::mutex backup_mutex;
};

class SaveBackupManager : public QObject {
    Q_OBJECT

public:
    explicit SaveBackupManager(QObject* parent = nullptr);
    ~SaveBackupManager();

    void setBackupEnabled(bool enabled);
    void setBackupInterval(int minutes);
    void setBackupDirectory(const QString& path);
    bool isBackupEnabled() const { return backup_enabled; }
    int getBackupInterval() const { return backup_interval; }
    QString getBackupDirectory() const { return backup_directory; }

public slots:
    void performBackup();

private:
    bool backup_enabled{false};
    int backup_interval{30}; // Default 30 minutes
    QString backup_directory;
    QTimer* backup_timer;

    bool createBackupDirectory(const std::filesystem::path& path);
    void copyDirectory(const std::filesystem::path& src, const std::filesystem::path& dst);
};

} // namespace Core
