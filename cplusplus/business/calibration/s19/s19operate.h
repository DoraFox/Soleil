#ifndef S19OPERATE_H
#define S19OPERATE_H

#include <QObject>
#include <cplusplus/midware/define/basedefine.h>
#include <QUrl>
#include <QRunnable>

class S19Operate : public  QRunnable
{
    using AddressPair = QPair<uint32_t, uint8_t>;
public:
    explicit S19Operate();

    void setReplaceInfo(const QUrl &url, QVector<int> address, QVector<int> value);

signals:

private:

    void startReplace();

    bool replaceValueByAddress(QByteArray &data, uint32_t address_input, uint8_t value);

    void run() override;

private:
    QUrl m_url;

    QVector<int> m_address;
    QVector<int> m_value;
};

#endif // S19OPERATE_H
