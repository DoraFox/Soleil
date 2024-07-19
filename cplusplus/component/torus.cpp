#include "torus.h"

#include <QPainter>
#include <QPainterPath>

Torus::Torus(QQuickItem *parent)
    :QQuickPaintedItem(parent)
{
    setAntialiasing(true);

    connect(this, &Torus::startAngleChanged, this, [this](){ update(); });
    connect(this, &Torus::arcLengthChanged, this, [this](){ update(); });

    connect(this, &Torus::strokeWidthChanged, this, [this](){ update(); });
    connect(this, &Torus::colorChanged, this, [this](){ update(); });
}

void Torus::paint(QPainter *painter)
{
    Q_CHECK_PTR(painter);

    static QPen pen;
    pen.setWidth(m_strokeWidth);
    pen.setColor(m_color);
    painter->setPen(pen);

    int x = m_strokeWidth / 2.0 + 0.5;
    double w = width() - 2 * x;
    double h = height() - 2 * x;

    QPainterPath path;
    path.arcMoveTo(x, x, w, h, m_startAngle);
    path.arcTo(x, x, w, h, m_startAngle, m_arcLength);
    painter->drawPath(path);
}
