#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <QImage>
#include <QObject>

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoader(QObject *parent = nullptr);
    static ImageLoader *instance();

    QImage load(const QString &id) const;
    
    QPair<unsigned char*, int> getProductImage(const QString &product_id) const;
    QImage loadAvatar(const QString &user_id) const;
};

#endif // IMAGELOADER_H
