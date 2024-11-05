// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "save_backup_manager.h"
#include <QDir>
#include <QFileInfo>
#include "common/config.h"
#include "common/path_util.h"

SaveBackupManager::SaveBackupManager(QObject* parent)
    : QObject(parent), m_backupTimer(new QTimer(this)), m_isEnabled(false) {
    connect(m_backupTimer, &QTimer::timeout, this, &SaveBackupManager::performBackup);
}

SaveBackupManager::~SaveBackupManager() {
    m_backupTimer->stop();
}

void SaveBackupManager::setBackupFolder(const QString& folder) {
    m_backupFolder = folder;
}

void SaveBackupManager::setBackupInterval(int minutes) {
    m_backupTimer->setInterval(minutes * 60 * 1000);
    updateTimer();
}

void SaveBackupManager::setEnabled(bool enabled) {
    m_isEnabled = enabled;
    updateTimer();
}

void SaveBackupManager::updateTimer() {
    if (m_isEnabled && !m_backupFolder.isEmpty()) {
        m_backupTimer->start();
    } else {
        m_backupTimer->stop();
    }
}

void SaveBackupManager::performBackup() {
    QString savesPath;
    Common::FS::PathToQString(savesPath, Common::FS::GetUserPath(Common::FS::PathType::SavesDir));
    QDir savesDir(savesPath);

    if (!savesDir.exists()) {
        return;
    }

    QDir backupDir(m_backupFolder);
    if (!backupDir.exists()) {
        backupDir.mkpath(".");
    }

    QDateTime now = QDateTime::currentDateTime();

    for (const QFileInfo& entry : savesDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString gameName = entry.fileName();
        QString backupName = createBackupName(gameName, now);
        QString backupPath = backupDir.filePath(backupName);

        QDir().mkpath(QFileInfo(backupPath).path());
        QDir(entry.filePath()).copyRecursively(backupPath);
    }
}

QString SaveBackupManager::createBackupName(const QString& gameName, const QDateTime& timestamp) const {
    return QString("%1_%2").arg(gameName, timestamp.toString("yyyyMMdd_HHmmss"));
}