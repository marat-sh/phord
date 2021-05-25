/**
 * Cache contains information about all mentioned files.
 * The information can be not full.
 * Cache loads it in backgroud, tracks changes and signals about changes.
 * Cache uses CacheLoader and CacheMonitor helper classes.
 *
 * Use get() to try getting cached information.
 * Cache returns everything what it has. And schedules loading not existed parts.
 * It is thread safe.
 *
 * Use noNeeded() to inform Cache about stop using some resource.
 * It allows Cache to release memory.
 * It is thread safe.
 */

#ifndef CACHE_H
#define CACHE_H

#include <QMap>
#include <QLinkedList>
#include <QReadWriteLock>
#include <QThread>

#include "cacheloader.h"
#include "cachemonitor.h"
#include "file.h"

class Cache : public QObject
{
    Q_OBJECT

public:
    friend class CacheLoader;
    friend class CacheMonitor;

    enum Usage
    {
        InProject = 0x1,
        InFileList = 0x2,
    };

    enum State
    {
        Loading,    // State between the first mention and load result.
        Updating,   // Cache contains previous version of the file.
        Loaded,     // Cache contains last version of the file.
        Error,      // File maybe deleted.
    };

    struct CacheItem
    {
        State state;
        Usage usage;
        QFileInfo info;
        QPixmap pixmap;
    };

    enum NoNeedReason
    {
        Deleted,                // Cache removes the item imediatelly.
        ExludedFromProject,     // Cache keeps the item while it is used in some model or there is enough space.
        ExludedFromFileList,    // Cache keeps the item while it is used in some model or there is enough space.
    };

    Cache(QObject *parent = nullptr);

    // Returns immediately. Some information can be empty.
    // Cache starts loading empty fields and will signal about updates.
    CacheItem get(const QString path);
    CacheItem get(const QFileInfo &fileInfo);

public slots:
    // All models have to inform Cache about ending of resource usage. It allows Cache destroy unused items.
    // Possible to use direct connect it is threadsafe.
    void noNeeded(const QString path, Cache::NoNeedReason reason);

signals:
    // Signals about loading or changing used resource.
    void updated(const QString path, bool deleted);

protected:
    // Destroy all unused items.
    void decay();

    // Updates Cache. Is called from CacheLoader.
    void insert(const QString &path, QFileInfo &&info, QPixmap &&pixmap);

    // Is called from CacheLoader.
    void error(const QString &path);

protected:
    QReadWriteLock lock;

    // Keeps all information. The key is file path.
    QMap<QString,File> map;

    CacheLoader loader;
    CacheMonitor monitor;
};

Q_DECLARE_METATYPE(Cache::NoNeedReason);

#endif // CACHE_H
