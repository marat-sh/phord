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
#include <QMutex>
#include <QThread>
#include <QFileInfo>
#include <QPixmap>

#include "cacheloader.h"
#include "cachemonitor.h"
#include "common.h"

class Cache : public QObject
{
    Q_OBJECT

public:
    friend class CacheLoader;
    friend class CacheMonitor;

    // The enum values can be combined.
    enum Usage
    {
        InProject = 0x1,
        InFileList = 0x2,
    };
    Q_DECLARE_FLAGS(Usages, Cache::Usage)

    enum Status
    {
        Loading,    // Status between the first mention and load result.
        Updating,   // Cache contains previous version of the file.
        Loaded,     // Cache contains last version of the file.
        Error,      // File maybe deleted.
    };

    struct CacheItem
    {
        Usages usages;  // Service field to track if the item still in use.

        Status status;  // Addititonal field.
        QFileInfo info; // Information field.
        QPixmap pixmap; // Information field.
    };

    Cache(QObject *parent = nullptr);

    // Returns immediately. Some information can be empty.
    // Cache starts loading empty fields and will signal about updates.
    CacheItem get(const QString &path, Usages usages, bool urgent);
    CacheItem get(const QFileInfo &fileInfo, Usages usages, bool urgent);

public slots:
    // All models have to inform Cache about ending of resource usage. It allows Cache destroy unused items.
    // Possible to use direct connect it is threadsafe.
    void noNeeded(const QString &path, Cache::Usages reason);

signals:
    // Signals about loading or changing used resource.
    void updated(const QString path, bool deleted);

protected:
    // Destroy all unused items.
    void decay();

    // Updates Cache. Is called from CacheLoader.
    void insert(const QString path, QFileInfo &&info, QPixmap &&pixmap);

    // Is called from CacheLoader and CacheMonitor.
    void error(const QString path);

    // Is called from CacheMonitor.
    void validate(const QString path, QFileInfo &&info);

protected:
    QMutex lock;

    // Keeps all information. The key is file path.
    QMap<QString,CacheItem> map;

    CacheLoader loader;
    CacheMonitor monitor;
};

Q_DECLARE_METATYPE(Cache::Usages)
Q_DECLARE_OPERATORS_FOR_FLAGS(Cache::Usages)
//MY_DECLARE_OPERATORS_FOR_FLAGS(Cache::Usages)

#endif // CACHE_H
