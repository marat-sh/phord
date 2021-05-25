/**
 * CacheLoader implements Cache delayed load feature.
 * The class process tasks from CacheLoader::loadList.
 * The class calls Cache methods directly when finish each task.
 * It runs with low priority to not affect to GUI thread.
 *
 * Call load() to schedule update.
 * Call cancel() to prevent loading of not needed files.
 */

#ifndef CACHELOADER_H
#define CACHELOADER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class Cache;

class CacheLoader : public QThread
{
    Q_OBJECT

public:
    explicit CacheLoader(Cache *cache);
    ~CacheLoader() override;

    // Schedules loading of specified file. Merges previous requests.
    // If urgent is true places the request in head of tasks query.
    void load(const QString &path, bool urgent);

    // Cancels loading of specified file.
    void cancel(const QString &path);

protected:
    void run() override;
    void loadFile(const QString &path);

protected:
    QMutex lock;
    QWaitCondition waitCondition;

    // Requests to load uncached files.
    std::list<QString> loadList;

    Cache *cache;
};

#endif // CACHELOADER_H
