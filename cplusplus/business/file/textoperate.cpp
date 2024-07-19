#include "textoperate.h"

TextOperate::TextOperate(QObject *parent)
    : QObject{parent}
{

}

QString TextOperate::getFuntionParamsList(const QString &input)
{
    QStringList textList = input.trimmed().split('\n');
    QString output;

    foreach(const QString& text, textList)
    {
        if(text.endsWith(");") || text.startsWith(QLatin1String("signal")))
        {
            int leftIdx = text.indexOf('(');
            int rightIdx = text.indexOf(')');
            QStringRef params = text.midRef(leftIdx + 1, rightIdx - leftIdx - 1);
            QVector<QStringRef> paramsVec = params.split(',');

            for(int i = 0; i < paramsVec.size(); ++i)
            {
                QStringRef param = paramsVec.at(i).trimmed();
                QVector<QStringRef> tempVec = param.split(' ');
                if(tempVec.size() >= 2)
                {
                    output.append(tempVec.at(1)).append(QLatin1String(", "));
                }
                else if(tempVec.size() == 1)
                {
                    output.append(tempVec.at(0)).append(QLatin1String(", "));
                }
            }
            if(output.size() > 2)
            {
                output.remove(output.size() - 2, 2);
                output.append('\n');
            }
        }
    }

    if(QLatin1Char('\n') == output.back())
    {
        output.remove(output.size() - 1, 1);
    }

    return output;
}

QString TextOperate::hexToDecimal(const QString &input)
{
    QStringList textList = input.trimmed().split(QRegExp("\\s+"));
    QString output;

    foreach(const QString& text, textList)
    {
        bool ok = false;
        int dec = text.toInt(&ok, 16);
        if(ok)
        {
            output.append(QString("%1\n").arg(dec));
        }
    }
    return output;
}
