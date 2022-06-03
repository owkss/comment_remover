#include "functions.h"

#include <QDir>
#include <QStringList>
#include <QDirIterator>

QStringList utilities::get_source_filelist(const QDir &dir, bool cpp, bool recursive)
{
    QStringList list;

    QStringList filter;
    filter << "*.c";
    if (cpp)
        filter << "*.cpp";

    QDirIterator it(dir.absolutePath(), filter, QDir::Files, recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);
    while (it.hasNext())
    {
        list << it.next();
    }

    return list;
}
