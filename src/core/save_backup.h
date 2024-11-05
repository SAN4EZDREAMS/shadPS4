#pragma once

#include <QObject>
#include <QTimer>
#include <QString>
#include <QDir>

class SaveBackup : public QObject {
    Q_OBJECT

public:
    explicit SaveBackup(QObject *parent = nullptr);

    void setBackupFolder(const QString &folder);
    void setBackupInterval(int minutes);
    void setEnabled(bool enabled);

public slots:
    void performBackup();

private:
    QTimer *m_timer;
    QString m_backupFolder;
    bool m_enabled;
};
