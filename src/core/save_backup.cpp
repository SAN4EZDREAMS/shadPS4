// src/core/save_backup.cpp

#include "save_backup.h"
#include <QDateTime>
#include <QFile>
#include <QFileInfo>

SaveBackup::SaveBackup(QObject *parent) : QObject(parent), m_enabled(false) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SaveBackup::performBackup);
}

void SaveBackup::setBackupFolder(const QString &folder) {
    m_backupFolder = folder;
}

void SaveBackup::setBackupInterval(int minutes) {
    m_timer->setInterval(minutes * 60 * 1000);
}

void SaveBackup::setEnabled(bool enabled) {
    m_enabled = enabled;
    if (m_enabled) {
        m_timer->start();
    } else {
        m_timer->stop();
    }
}

void SaveBackup::performBackup() {
    if (!m_enabled) return;

    QDir saveDir(QDir::homePath() + "/.shadPS4/saves");
    QDir backupDir(m_backupFolder);

    if (!backupDir.exists()) {
        backupDir.mkpath(".");
    }

    QDateTime now = QDateTime::currentDateTime();
    QString backupFolderName = now.toString("yyyy-MM-dd_HH-mm-ss");
    QDir backupSubDir(backupDir.filePath(backupFolderName));
    backupSubDir.mkpath(".");

    QFileInfoList saveFiles = saveDir.entryInfoList(QDir::Files);
    for (const QFileInfo &fileInfo : saveFiles) {
        QString destPath = backupSubDir.filePath(fileInfo.fileName());
        QFile::copy(fileInfo.filePath(), destPath);
    }
}
