#include "fileoperate.h"

#include "cplusplus/midware/define/pathdefine.h"

#include <QByteArray>
#include <QTextStream>
#include <QRegExp>

FileOperate::FileOperate(QObject *parent)
    : QObject{parent}
{

}

void FileOperate::transformQMLConnection(const QUrl &url, bool recursive)
{
    // Connections{}  5.12 -> 5.15

    QStringList pathList;
    getAllFiles(pathList, url, QStringList({"*.qml"}), true);

    QRegExp reg("\\s+Connections");

    QRegExp regReplace("\\s+on\\S+:");

    int size = pathList.size();

    for(int i = 0; i < size; ++i)
    {
        auto path = pathList.at(i);
        QFile file(path);
        if(!file.open(QIODevice::ReadWrite))
        {
            DEBUGPREFIX << "error file path:" << path;
            continue;
        }

        QTextStream out(&file);
        out.setCodec("UTF-8");

        QString ret;
        QTextStream in(&ret);
        in.setCodec("UTF-8");

        while(!out.atEnd())
        {
            int leftBraceCount = 0;

            bool braceStart = false;
            QString line = out.readLine();
            if(line.contains(reg))
            {
                while(1)
                {
                    leftBraceCount += line.count('{');

                    if(braceStart)
                    {
                        leftBraceCount -= line.count('}');
                        if(leftBraceCount > 0)
                        {
                            if(line.contains(regReplace))
                            {
                                int idx = line.indexOf("on");
                                line.insert(idx, "function ");
                                idx = line.indexOf(':');
                                line.remove(idx, 1);
                                line.insert(idx, "()");
                            }
                        }
                        else
                        {
                            in << line << '\n';
                            break;
                        }
                    }
                    else
                    {
                        if(leftBraceCount != 0)
                        {
                            braceStart = true;
                            leftBraceCount -= line.count('}');
                        }
                    }
                    in << line << '\n';

                    line = out.readLine();
                }

            }
            else
            {
                in << line << "\n";
            }
        }

        file.close();

        if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            DEBUGPREFIX << "error:" << path;
            continue;
        }

        DEBUGPREFIX << "string wrote to file size:" << ret.size();
        file.write(ret.toUtf8());
        file.close();
    }
}

void FileOperate::getAllFiles(QStringList &pathList, const QUrl &url, const QStringList &nameFilters, bool recursive)
{
    QDir dir(url.toLocalFile().append('/'));

    QFileInfoList fileInfoList = dir.entryInfoList(nameFilters, QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    int size = fileInfoList.size();
    for(int i = 0; i < size; ++i)
    {
        auto fileInfo = fileInfoList.at(i);
        if(fileInfo.isDir())
        {
            getAllFiles(pathList, QUrl::fromLocalFile(fileInfo.absoluteFilePath()), nameFilters, recursive);
        }
        else
        {
            pathList.append(fileInfo.absoluteFilePath());
        }
    }
}
