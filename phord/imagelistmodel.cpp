#include <QDebug>

#include "imagelistmodel.h"

ImageListModel::ImageListModel(QObject *parent) : QObject(parent)
{

}

int ImageListModel::count() const
{
    return list.size();
}

void ImageListModel::insert(int idx, const QString &filePath, const QPixmap &pixmap)
{
    if (idx < 0){
        idx = 0;
    }
    if (idx > list.size()){
        idx = list.size();
    }
    list.insert(idx, pixmap);

    emit updated(idx);
}

const QPixmap* ImageListModel::data(int idx) const
{
    return (idx < list.size()) ? &list[idx] : nullptr;
}
