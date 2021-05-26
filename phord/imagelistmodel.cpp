#include <QDebug>

#include "imagelistmodel.h"

ImageListModel::ImageListModel(Cache *cache, QObject *parent) : QObject(parent)
{
    ImageListModel::cache = cache;
}

int ImageListModel::count() const
{
    return list.size();
}

void ImageListModel::insert(int idx, const QString &filePath)
{
    if (idx < 0){
        idx = 0;
    }
    if (idx > list.size()){
        idx = list.size();
    }
    list.insert(idx, filePath);

    emit updated(idx);
}

QPixmap ImageListModel::data(int idx) const
{
    if (idx < list.size()){
        Cache::CacheItem item = cache->get(list[idx], Cache::Usage::InProject, true);
        return item.pixmap;
    }

    return QPixmap();
}
