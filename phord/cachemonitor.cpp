#include <algorithm>
#include <utility>

#include <QDebug>
#include <QMutexLocker>
#include <QElapsedTimer>

#include "cachemonitor.h"
#include "cache.h"

CacheMonitor::CacheMonitor(Cache *cache) : QThread(cache)
{
    CacheMonitor::cache = cache;
    start(QThread::LowestPriority);
}

CacheMonitor::~CacheMonitor()
{
    requestInterruption();
    wait();
}

void CacheMonitor::add(const QString &path)
{
    QMutexLocker locker(&lock);

    // Searching place in ordered list.
    for (auto it=monitorList.cbegin(); it != monitorList.cend(); ++it){
        if (*it > path){
            // path is new item.
            monitorList.insert(it, path);
            break;
        }

        if (*it == path){
            // Do noting if duplication.
            return;
        }
    }

    // All items are lower than path.
    monitorList.push_back(path);

    // First item.
    if (monitorList.size() == 1){
        currentPosition = monitorList.cbegin();
    }
}

void CacheMonitor::remove(const QString &path)
{
    QMutexLocker locker(&lock);

    auto it = std::find(monitorList.cbegin(), monitorList.cend(), path);
    if (it != monitorList.cend()){
        // After removing currentPosition has to stay valid.
        // Except case with list with one item.
        if (it == currentPosition){
            shiftCurrent();
        }

        monitorList.erase(it);
    }
}

bool CacheMonitor::shiftCurrent()
{
    ++currentPosition;
    if (currentPosition == monitorList.cend()){
        currentPosition = monitorList.cbegin();
        return true;
    }

    return false;
}

void CacheMonitor::run()
{
    QString path;
    bool newLoop;

    QElapsedTimer timer;
    timer.start();

    // Running until end of the application.
    while (!isInterruptionRequested()){

        // Retrieve path if it there.
        lock.lock();

        if (monitorList.empty()){
            newLoop = true;
        } else {
            path = *currentPosition;
            newLoop = shiftCurrent();
        }

        lock.unlock();

        // Process task out of lock.
        testFile(path);

        // Spend stable time for each scan loop.
        if (newLoop){
            const qint64 period = 500;
            qint64 t = timer.restart();
            if (period > t){
                msleep(static_cast<unsigned long>(period - t));
            }
        }
    }
}

void CacheMonitor::testFile(const QString &path)
{
    QFileInfo info(path);

    if (info.isFile()){
        cache->validate(path, std::move(info));
    } else {
        cache->error(path);
    }
}
