#ifndef IMAGELISTVIEW_H
#define IMAGELISTVIEW_H

#include <QFrame>

#include "imagelistmodel.h"
#include "common.h"

class ImageListView : public QFrame
{
    Q_OBJECT

public:
    explicit ImageListView(QWidget *parent = nullptr);

    void setModel (ImageListModel *model);
    int indexAt(const QPoint &pos) const;

signals:

private slots:
    void model_updated(int idx);

protected:
    void paintEvent(QPaintEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    const int gridWidth = (THM_FULL_WIDTH) + 1;
    const int gridHeight = (THM_FULL_HEIGHT) + 1;

    ImageListModel *model;
};

#endif // IMAGELISTVIEW_H
