#include <algorithm>
#include <utility>

#include <QDebug>
#include <QMutexLocker>

#include "cacheloader.h"
#include "cache.h"
#include "common.h"

CacheLoader::CacheLoader(Cache *cache) : QThread(cache)
{
    CacheLoader::cache = cache;
    start(QThread::LowPriority);
}

CacheLoader::~CacheLoader()
{
    requestInterruption();
    waitCondition.wakeOne();
    wait();
}

void CacheLoader::load(const QString &path, bool urgent)
{
    QMutexLocker locker(&lock);

    // Checking request duplication.
    auto it = std::find(loadList.cbegin(), loadList.cend(), path);
    if (it != loadList.cend()){
        if (urgent){
            // Move previous request for the same path to head.
            loadList.splice(loadList.cbegin(), loadList, it);
        }

        // Keep previous request for the same path without changes.
        return;
    }

    // There is not duplication in requests.
    if (urgent){
        loadList.push_front(path);
    } else {
        loadList.push_back(path);
    }

    // Wake up loading thread if it finished all tasks.
    waitCondition.wakeOne();
}

void CacheLoader::cancel(const QString &path)
{
    QMutexLocker locker(&lock);

    auto it = std::find(loadList.cbegin(), loadList.cend(), path);
    if (it != loadList.cend()){
        loadList.erase(it);
    }
}

void CacheLoader::run()
{
    QString path;

    // Running until end of the application.
    while (!isInterruptionRequested()){

        // Retrieve task if it there. Or wait task if it is not there.
        lock.lock();

        if (loadList.empty()){
            waitCondition.wait(&lock);
        } else {
            path = std::move(loadList.front());
            loadList.pop_front();
        }

        lock.unlock();

        // Process task out of lock.
        loadFile(path);
    }
}

void CacheLoader::loadFile(const QString &path)
{
    QFileInfo info(path);

    if (!info.isFile()){
        cache->error(path);
        return;
    }

    QPixmap pixmap(path);
    if (!pixmap.isNull()){
        pixmap = pixmap.scaled(THM_IMG_WIDTH, THM_IMG_HEIGHT,
                               Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    cache->insert(path, std::move(info), std::move(pixmap));
}
