#ifndef FILE_H
#define FILE_H

#include <QFileInfo>
#include <QPixmap>

class File
{
public:
    File() = default;
    File(const QFileInfo &info);

    const QFileInfo &info() const { return info_; }

    QPixmap pixmap() const;

private:
    QFileInfo info_;

    // Created when asked first time.
    mutable QPixmap pixmap_;
};

#endif // FILE_H
