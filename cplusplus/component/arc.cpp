#include "arc.h"

#include <QPainter>
#include <QPainterPath>
#include <QSvgRenderer>

static const QString svgSource = QString::fromUtf8(R"(

)");

Arc::Arc(QQuickItem *parent)
    :QQuickPaintedItem(parent)
{
    setAntialiasing(true);
    setSmooth(true);

    connect(this, &Arc::startAngleChanged, this, [this](){ update(); });
    connect(this, &Arc::arcLengthChanged, this, [this](){ update(); });

    connect(this, &Arc::strokeWidthChanged, this, [this](){ update(); });
    connect(this, &Arc::colorChanged, this, [this](){ update(); });
}

void Arc::paint(QPainter *painter)
{
    Q_CHECK_PTR(painter);

#ifdef USE_ARC
    static QPen pen;
    pen.setWidth(m_strokeWidth);
    pen.setColor(m_color);
    painter->setPen(pen);

    int x = m_strokeWidth / 2 + 0.5;
    double w = width() - x * 2;
    double h = height() - x * 2;

    QPainterPath path1;
    path1.arcMoveTo(x, x, w, h, m_startAngle);
    path1.arcTo(x, x, w, h, m_startAngle, m_arcLength);
    //painter->drawPath(path1);


    pen.setColor(m_color2);
    painter->setPen(pen);

    QPainterPath path2;
    double secondStartAngle = m_startAngle + m_arcLength;
    path2.arcMoveTo(x, x, w, h, secondStartAngle);
    path2.arcTo(x, x, w, h, secondStartAngle, 360 - m_arcLength);
    //painter->drawPath(path2);
#endif

#ifdef USE_SVG
    bool ret = m_svgRenderer->load(svgSource.toUtf8());

    if(ret)
    {
        m_svgRenderer->render(painter);
    }
#endif

    int x = width() / 2 + 0.5;
    int y = height() / 2 + 0.5;
    int w = 2 * x;
    int h = 2 * y;
    QRect rect(-x, -y, w, h);

    QPainterPath mainPath;
    mainPath.arcTo(rect, startAngle(), arcLength());

    QPainterPath subPath;
    subPath.addEllipse(rect.adjusted(strokeWidth(), strokeWidth(), -strokeWidth(), -strokeWidth()));

    mainPath = mainPath - subPath;

    QBrush brush(color());
    painter->setBrush(brush);
    painter->setPen(Qt::NoPen);

    painter->translate(x, y);
    painter->drawPath(mainPath);
}
