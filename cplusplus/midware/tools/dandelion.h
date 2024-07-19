#ifndef DANDELION_H
#define DANDELION_H

#include <QObject>
#include <QColor>

class Dandelion : public QObject
{
    Q_OBJECT
public:
    explicit Dandelion(QObject *parent = nullptr);

    bool isLightColor(int (&rgb)[3]);

    bool isDeepColor(int (&rgb)[3]);

    Q_INVOKABLE void makeDirectory(QString dirPath);

    Q_INVOKABLE QString getDirectoryAbsolutePath(QString path);

    const char convertU82Char(uint8_t value);


public slots:

    QColor getLightColor();

    QColor getDeepColor();

signals:

};

#endif // DANDELION_H
