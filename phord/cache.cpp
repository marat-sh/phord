#include <QDebug>
#include <QMutexLocker>

#include "cache.h"

Cache::Cache(QObject *parent) : QObject(parent), loader(this), monitor(this)
{
}

Cache::CacheItem Cache::get(const QString &path, Usages usages, bool urgent)
{
    // Simple mutex simplifies the function significantly rather than QReadWriteLock.
    QMutexLocker locker(&lock);

    bool isPresented = map.contains(path);
    CacheItem &item_ref = map[path];

    item_ref.usages |= usages;

    if (!isPresented){
        item_ref.status = Status::Loading;
        loader.load(path, urgent);
        monitor.add(path);
    } else if (item_ref.status == Status::Loading || item_ref.status == Status::Updating){
        loader.load(path, urgent);
    }

    return item_ref;
}

Cache::CacheItem Cache::get(const QFileInfo &fileInfo, Usages usages, bool urgent)
{
    const QString path = fileInfo.filePath();

    QMutexLocker locker(&lock);

    bool isPresented = map.contains(path);
    CacheItem &item_ref = map[path];

    item_ref.usages |= usages;

    if (!isPresented){
        item_ref.status = Status::Loading;
        item_ref.info = fileInfo;
        loader.load(path, urgent);
        monitor.add(path);
    } else if (item_ref.status == Status::Loading || item_ref.status == Status::Updating){
        loader.load(path, urgent);
    }

    return item_ref;
}

void Cache::noNeeded(const QString &path, Cache::Usages reasons)
{
    QMutexLocker locker(&lock);

    if (map.contains(path)){
        CacheItem &item_ref = map[path];

        item_ref.usages &= ~reasons;

        if (! item_ref.usages){
            loader.cancel(path);
            monitor.remove(path);

            // TODO don't remove CacheItem immediately.
            map.remove(path);
        }
    }
}

void Cache::decay()
{
}

void Cache::insert(const QString path, QFileInfo &&info, QPixmap &&pixmap)
{
    QMutexLocker locker(&lock);

    CacheItem &item_ref = map[path];

    item_ref.status = Status::Loaded;
    item_ref.info = info;
    item_ref.pixmap = pixmap;

    locker.unlock();
    emit updated(path, false);
}

void Cache::error(const QString path)
{
    QMutexLocker locker(&lock);

    CacheItem &item_ref = map[path];

    item_ref.status = Status::Error;
    item_ref.pixmap = QPixmap();

    locker.unlock();
    emit updated(path, true);
}

void Cache::validate(const QString path, QFileInfo &&info)
{
    QMutexLocker locker(&lock);

    if (!map.contains(path)){
        return;
    }
    CacheItem &item_ref = map[path];

    if (item_ref.info == info){
        return;
    }

    item_ref.status = Status::Updating;
    item_ref.info = info;

    loader.load(path, false);

    locker.unlock();
    emit updated(path, false);
}
