// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <QTimer>
#include <QString>
#include <QDateTime>

class SaveBackupManager : public QObject {
    Q_OBJECT

public:
    explicit SaveBackupManager(QObject* parent = nullptr);
    ~SaveBackupManager();

    void setBackupFolder(const QString& folder);
    void setBackupInterval(int minutes);
    void setEnabled(bool enabled);

public slots:
    void performBackup();

private:
    QTimer* m_backupTimer;
    QString m_backupFolder;
    bool m_isEnabled;

    void updateTimer();
    QString createBackupName(const QString& gameName, const QDateTime& timestamp) const;
};