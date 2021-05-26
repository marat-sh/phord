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

    int count() const;
    void insert(int idx, const QString &filePath);
    QPixmap data(int idx) const;

signals:
    void updated (int idx);

private:
    QStringList list;
    Cache *cache;
};

#endif // IMAGELIST_H
