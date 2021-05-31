#ifndef IMAGELIST_H
#define IMAGELIST_H

#include <QObject>
#include <QPixmap>

#include "cache.h"

class ImageListModel : public QObject
{
    Q_OBJECT

public:
    explicit ImageListModel(Cache *cache, QObject *parent = nullptr);

    void setNames(QStringList &&names);
    QStringList names();

    int count() const;
    void insert(int idx, const QStringList &filePaths);
    QPixmap data(int idx) const;

signals:
    void updated (int beginIdx, int endIdx);

private slots:
    void cache_updated(const QString path, bool deleted);

private:
    QStringList list;
    Cache *cache;
};

#endif // IMAGELIST_H
