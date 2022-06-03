#ifndef FUNCTIONS_H
#define FUNCTIONS_H

class QDir;
class QStringList;

namespace utilities
{
    QStringList get_source_filelist(const QDir &dir, bool cpp, bool recursive);
}

#endif // FUNCTIONS_H
