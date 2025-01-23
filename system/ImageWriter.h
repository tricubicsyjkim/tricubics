#ifndef IMAGEWRITER_H
#define IMAGEWRITER_H

#include <stdint.h>
#include <stdlib.h>

#include <QDataStream>
#include <QFile>
#include <QDir>
#include <QDate>
#include <QString>
#include <QSharedPointer>

enum e_imageType {
    bmp,
    ppm,
    jpg,
    unknown
};

struct V4L2Parameters;

struct ImageWriter
{
public:
    ImageWriter(const V4L2Parameters& param, char* data, size_t len, e_imageType type);
    static QSharedPointer<ImageWriter> ImageSave(const V4L2Parameters& param, char* data, size_t len, e_imageType type=e_imageType::ppm);
    static e_imageType imageType(const QString& img);
    static uint32_t imageFormat(const QString& img);
    QString findCamId(const QString devName);
    QString path_;
};

#endif // IMAGEWRITER_H
