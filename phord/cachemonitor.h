/**
 * CacheMonitor is part of Cache and manipulates with Cache private fields directly.
 * The class iterates through all files and checks if their was changed.
 * The class doesn't use QFileSystemWatcher to keep system resources.
 * It runs with lowest priority to not affect to GUI and other thread.
 */

#ifndef CACHEMONITOR_H
#define CACHEMONITOR_H

#include <QThread>

class Cache;

class CacheMonitor : public QThread
{
    Q_OBJECT
public:
    explicit CacheMonitor(QObject *parent = nullptr);

signals:

protected:
    //
    std::list<QString> monitorList;

    Cache *cache;
};

#endif // CACHEMONITOR_H
