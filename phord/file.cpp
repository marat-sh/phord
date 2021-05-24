#include <QDir>

#include "file.h"
#include "common.h"

File::File(const QFileInfo &info)
{
    info_ = info;
}

QPixmap File::pixmap() const
{
    if (pixmap_.isNull()){
        QPixmap p(info_.filePath());
        pixmap_ = p.scaled(THM_IMG_WIDTH, THM_IMG_HEIGHT, Qt::KeepAspectRatio);
    }

    return pixmap_;
}
