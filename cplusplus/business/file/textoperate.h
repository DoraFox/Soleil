#ifndef TEXTOPERATE_H
#define TEXTOPERATE_H

#include <QObject>

class TextOperate : public QObject
{
    Q_OBJECT
public:
    explicit TextOperate(QObject *parent = nullptr);

    Q_INVOKABLE QString getFuntionParamsList(const QString& input);

    Q_INVOKABLE QString hexToDecimal(const QString& input);
};

#endif // TEXTOPERATE_H
