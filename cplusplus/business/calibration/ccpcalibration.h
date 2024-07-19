#ifndef CCPCALIBRATION_H
#define CCPCALIBRATION_H

#include <QObject>

class CCPCalibration : public QObject
{
    Q_OBJECT
public:
    explicit CCPCalibration(QObject *parent = nullptr);

signals:
};

#endif // CCPCALIBRATION_H
