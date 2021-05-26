/**
 * FileListModel presents list of files in specified directory for QListView running in icon mode.
 *
 * The class takes file list with QDir.
 * Monitor changes in the list with QFileSystemWatcher.
 * Get file contents from Cache.
 *
 * The class makes additional directory scan after some time after QFileSystemWatcher signal
 * to show final state (not transient).
 */

#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QAbstractListModel>
#include <QFileSystemWatcher>
#include <QFileInfoList>
#include <QTimer>

#include "cache.h"

class FileListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit FileListModel(Cache *cache, QObject *parent = nullptr);

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

    void cache_updated(const QString path, bool deleted);

private:
    void update();

private:
    QString path;

    // Keeps last scan info.
    QFileInfoList fileList;

    Cache *cache;

    // Monitors directory changes.
    QFileSystemWatcher fileSystemWatcher;

    // Used to make additional scan after notify.
    QTimer updateTimer;
};

#endif // FILELISTMODEL_H
