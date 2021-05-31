#include <QDebug>

#include "imagelistmodel.h"

ImageListModel::ImageListModel(Cache *cache, QObject *parent) : QObject(parent)
{
    ImageListModel::cache = cache;
    connect(cache, &Cache::updated, this, &ImageListModel::cache_updated);
}

void ImageListModel::setNames(QStringList &&names)
{
    for (const QString &name: list){
        cache->noNeeded(name, Cache::Usage::InProject);
    }

    list = std::move(names);

    for (const QString &name: list){
        cache->get(name, Cache::Usage::InProject, false);
    }

    emit updated(0, list.size());
}

QStringList ImageListModel::names()
{
    return list;
}

int ImageListModel::count() const
{
    return list.size();
}

QPixmap ImageListModel::data(int idx) const
{
    if (idx < list.size()){
        Cache::CacheItem item = cache->get(list[idx], Cache::Usage::InProject, true);
        return item.pixmap;
    }

    return QPixmap();
}

void ImageListModel::insert(int idx, const QStringList &filePaths)
{
    if (idx < 0){
        idx = 0;
    }
    if (idx > list.size()){
        idx = list.size();
    }

    for (const QString &path : filePaths){
        list.insert(idx, path);
    }

    emit updated(idx, idx+filePaths.size());
}

void ImageListModel::cache_updated(const QString path, bool deleted)
{
    emit updated(0, list.size());
}

