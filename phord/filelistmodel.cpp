#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <QSize>
#include <QPixmap>
#include <QMimeData>

#include "filelistmodel.h"
#include "common.h"

FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    updateTimer.setInterval(500);
    updateTimer.setSingleShot(true);

    connect(&fileSystemWatcher, &QFileSystemWatcher::directoryChanged,
            this, &FileListModel::watcher_directoryChanged);

    connect(&updateTimer, &QTimer::timeout,
            this, &FileListModel::updateTimer_timeout);
}

int FileListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return fileList.size();
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
        case Qt::DisplayRole:
            return fileList[index.row()].info().fileName();
        case Qt::TextAlignmentRole:
            return Qt::AlignBottom + Qt::AlignHCenter;
        case Qt::DecorationRole:
            return fileList[index.row()].pixmap();
        case Qt::SizeHintRole:
            return QSize(THM_FULL_WIDTH, THM_FULL_HEIGHT);
    }
    return QVariant();
}

Qt::ItemFlags FileListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractListModel::flags(index);

    if (index.isValid()){
        flags |= Qt::ItemIsDragEnabled;
    }

    return flags;
}

QStringList FileListModel::mimeTypes() const
{
    QStringList types;
    types << THM_DRAG_DROP_MIME_TYPE;
    return types;
}

QMimeData *FileListModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    // TODO move mimeData actions to one file.
    for (QModelIndex index : indexes) {
        if (index.isValid()) {
            const File &file = fileList[index.row()];
            stream << file.info().filePath() << file.pixmap();
        }
    }

    mimeData->setData(THM_DRAG_DROP_MIME_TYPE, encodedData);
    return mimeData;
}

Qt::DropActions FileListModel::supportedDragActions() const
{
    return Qt::CopyAction;
}

// reset directory watcher
// clear old information
// read files' set
void FileListModel::setDir(const QString &path)
{
    if (FileListModel::path == path){
        return;
    }
    FileListModel::path = path;

    QStringList oldDirs = fileSystemWatcher.directories();

    if (!oldDirs.isEmpty()){
        fileSystemWatcher.removePaths(oldDirs);
    }

    fileSystemWatcher.addPath(path);

    QDir dir(path);
    QFileInfoList fileInfoListNew = dir.entryInfoList(QDir::Files, QDir::Name);

    beginResetModel();
    fileList.clear();
    for (const QFileInfo &fi : fileInfoListNew){
        fileList.append(File(fi));
    }
    endResetModel();

    updateTimer.start();
}

// remove rows associated with deleted files
// add rows for new files
// update rows for updated files
void FileListModel::watcher_directoryChanged(const QString &path)
{
    FileListModel::path = path;
    update();
    updateTimer.start();
}

void FileListModel::updateTimer_timeout()
{
    update();
}

void FileListModel::update()
{
    QDir dir(path);
    QFileInfoList fileInfoListNew = dir.entryInfoList(QDir::Files, QDir::Name);

    // Comparison between two sorted lists.
    QFileInfoList::const_iterator itNew = fileInfoListNew.cbegin();
    int iOld = 0;
    while (iOld < fileList.size() && itNew != fileInfoListNew.cend()){
        File &fileOld = fileList[iOld];

        if (fileOld.info() == *itNew){
            // Keep unchanged items.
            ++iOld;
            ++itNew;
        } else if (fileOld.info().fileName() == itNew->fileName()) {
            // Update existed item.
            fileOld = *itNew;
            emit dataChanged(index(iOld), index(iOld));
            ++iOld;
            ++itNew;
        } else if (fileOld.info().fileName() < itNew->fileName()) {
            // Remove old item.
            beginRemoveRows(QModelIndex(), iOld, iOld);
            fileList.removeAt(iOld);
            endRemoveRows();
        } else {
            // Add new item.
            beginInsertRows(QModelIndex(), iOld, iOld);
            fileList.insert(iOld, *itNew);
            endInsertRows();
        }
    }

    // Deal with tail.
    if (iOld < fileList.size()){
        // Remove last items.
        beginRemoveRows(QModelIndex(), iOld, fileList.size()-1);
        while (iOld < fileList.size()){
            fileList.removeAt(iOld);
            ++iOld;
        }
        endRemoveRows();
    } else if (itNew != fileInfoListNew.cend()){
        // Add last items.
        beginInsertRows(QModelIndex(), iOld, iOld + (fileInfoListNew.cend() - itNew));
        while (itNew != fileInfoListNew.cend()){
            fileList.append(*itNew);
            ++itNew;
        }
        endInsertRows();
    }
}
