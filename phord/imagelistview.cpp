#include <QDebug>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPainter>

#include "imagelistview.h"
#include "common.h"

ImageListView::ImageListView(QWidget *parent) : QFrame(parent)
{
}

void ImageListView::setModel(ImageListModel *model)
{
    ImageListView::model = model;
    connect(model, &ImageListModel::updated, this, &ImageListView::model_updated);
    update();
}

int ImageListView::indexAt(const QPoint &pos) const
{
    int nx = width() / gridWidth;

    int x = pos.x() / gridWidth;
    int y = pos.y() / gridHeight;

    int idx = y*nx + x;

    return idx;
}

void ImageListView::model_updated(int beginIdx, int endIdx)
{
    update();
}

void ImageListView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    int nx = width() / gridWidth;

    for (int i=0; i<model->count(); ++i){
        QPixmap pixmap = model->data(i);
        if (!pixmap.isNull()){
            painter.drawPixmap(i % nx * gridWidth, (i / nx) * gridHeight, pixmap);
        }
    }
}

void ImageListView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(THM_DRAG_DROP_MIME_TYPE)) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void ImageListView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(THM_DRAG_DROP_MIME_TYPE)) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void ImageListView::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(THM_DRAG_DROP_MIME_TYPE)) {
        QByteArray itemData = event->mimeData()->data(THM_DRAG_DROP_MIME_TYPE);
        QDataStream stream(&itemData, QIODevice::ReadOnly);

        QStringList filePaths;
        forever {
            QString path;
            stream >> path;
            if (!path.isEmpty()){
                filePaths += path;
            } else {
                break;
            }
        }

        model->insert(indexAt(event->pos()), filePaths);

        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}
