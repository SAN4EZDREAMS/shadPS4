// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

namespace Core {

class SaveBackupManager : public QObject {
    Q_OBJECT

public:
    explicit SaveBackupManager(QObject* parent = nullptr);
    ~SaveBackupManager();

    void setBackupEnabled(bool enabled);
    void setBackupInterval(int minutes);
    void setBackupDirectory(const QString& path);

private slots:
    void performBackup();

private:
    bool createBackupDirectory(const QString& path);
    void copyDirectory(const QString& src, const QString& dst);

    QTimer* backup_timer;
    bool backup_enabled = false;
    int backup_interval = 60; // Default to 60 minutes
    QString backup_directory;
};

} // namespace Core
