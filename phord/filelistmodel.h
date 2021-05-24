#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QAbstractListModel>
#include <QFileSystemWatcher>
#include <QFileInfoList>
#include <QTimer>

#include "file.h"

class FileListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit FileListModel(QObject *parent = nullptr);

    // Usuall model interface.
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // To drag thumbnails from the view.
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    Qt::DropActions supportedDragActions() const override;

public slots:
    // Changes showed directory.
    void setDir(const QString &path);

private slots:
    // Updates model, when directory's file set is changed.
    void watcher_directoryChanged(const QString &path);

    void updateTimer_timeout();

private:
    void update();

private:
    QString path;

    // Monitors directory changes.
    QFileSystemWatcher fileSystemWatcher;

    // Keeps last scan info.
    QList<File> fileList;

    // Used to make additional scan after notify.
    QTimer updateTimer;
};

#endif // FILELISTMODEL_H