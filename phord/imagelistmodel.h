#ifndef IMAGELIST_H
#define IMAGELIST_H

#include <QObject>
#include <QPixmap>

class ImageListModel : public QObject
{
    Q_OBJECT

public:
    explicit ImageListModel(QObject *parent = nullptr);

    int count() const;
    void insert(int idx, const QString &filePath, const QPixmap &pixmap);
    const QPixmap* data(int idx) const;

signals:
    void updated (int idx);

private:
    QList<QPixmap> list;
};

#endif // IMAGELIST_H
