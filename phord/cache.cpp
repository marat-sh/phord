#include "cache.h"

Cache::Cache(QObject *parent) : QObject(parent), loader(this), monitor(this)
{

}

Cache::CacheItem Cache::get(const QString path)
{

}

Cache::CacheItem Cache::get(const QFileInfo &fileInfo)
{

}

void Cache::noNeeded(const QString path, NoNeedReason reason)
{

}

void Cache::decay()
{

}

void Cache::insert(const QString &path, QFileInfo &&info, QPixmap &&pixmap)
{

}

void Cache::error(const QString &path)
{

}
