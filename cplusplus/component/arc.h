#ifndef ARC_H
#define ARC_H

#include <QQuickPaintedItem>
#include <midware/define/basedefine.h>
#include <QColor>

class Arc : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(double startAngle READ startAngle WRITE set_startAngle NOTIFY startAngleChanged)
    Q_PROPERTY(double arcLength READ arcLength WRITE set_arcLength NOTIFY arcLengthChanged)
    Q_PROPERTY(double strokeWidth READ strokeWidth WRITE set_strokeWidth NOTIFY strokeWidthChanged)
    Q_PROPERTY(QColor color READ color WRITE set_color NOTIFY colorChanged)
public:
    explicit Arc(QQuickItem* parent = nullptr);

    void paint(QPainter *painter) override;

signals:
    DECLARE_PROPERTY_SIGNAL(double, startAngle)
    DECLARE_PROPERTY_SIGNAL(double, arcLength)
    DECLARE_PROPERTY_SIGNAL(int, strokeWidth)
    DECLARE_PROPERTY_SIGNAL(QColor, color)

private slots:


private:

    DECLARE_QML_PROPERTY(double, startAngle)
    DECLARE_QML_PROPERTY(double, arcLength)
    DECLARE_QML_PROPERTY(int, strokeWidth)
    DECLARE_QML_PROPERTY(QColor, color)

};


#endif // ARC_H
