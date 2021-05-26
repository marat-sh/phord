/**
 * CacheMonitor iterates through CacheMonitor::monitorList and checks if each file was changed.
 * The class calls Cache methods directly when finish each task.
 * The class doesn't use QFileSystemWatcher to keep system resources.
 * It runs with lowest priority to not affect to GUI and other thread.
 * The class orders requests to file system.
 */

#ifndef CACHEMONITOR_H
#define CACHEMONITOR_H

#include <QThread>
#include <QMutex>

class Cache;

class CacheMonitor : public QThread
{
    Q_OBJECT

public:
    explicit CacheMonitor(Cache *cache);
    ~CacheMonitor() override;

    // Add file to the monitor. It does nothing if file was there.
    void add(const QString &path);

    // Remove specified file from the monitor.
    void remove(const QString &path);

protected:
    void run() override;

    // Shifts currentPosition to the next item in ring buffer.
    // Don't call it for empty list.
    // Returns true when starts new loop.
    bool shiftCurrent();

    // Read file info.
    void testFile(const QString &path);

protected:
    QMutex lock;

    // Ordered list with all mentioned files. Is used as ring buffer.
    std::list<QString> monitorList;
    std::list<QString>::const_iterator currentPosition;

    Cache *cache;
};

#endif // CACHEMONITOR_H
