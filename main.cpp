#include <iostream>

#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QCommandLineParser>

#include "functions.h"
#include "traverser.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("comment_remover");
    QCoreApplication::setApplicationVersion("1.0.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Remove comments frome the inside of function's body");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption dir_option(QStringList() << "d" << "dir", QCoreApplication::translate("main", "<directory> where the files are located"), QCoreApplication::translate("main", "directory"));
    parser.addOption(dir_option);

    QCommandLineOption cpp_option("c", QCoreApplication::translate("main", "Include .cpp files"));
    parser.addOption(cpp_option);

    QCommandLineOption recursive_option("r", QCoreApplication::translate("main", "Recursive traverse"));
    parser.addOption(recursive_option);

    parser.process(app);

    QDir dir = parser.value(dir_option);
    const bool recursive = parser.isSet(recursive_option);
    const bool cpp = parser.isSet(cpp_option);

    if (!dir.exists())
    {
        std::cerr << "main(): directory does not exists" << std::endl;
        return 1;
    }

    QStringList list = utilities::get_source_filelist(dir, cpp, recursive);
    for (int i = 0; i < list.size(); ++i)
    {
        const QByteArray &str = list.at(i).toLocal8Bit();
        const char *filename = str.constData();

        if (!traverser::parse(filename))
        {
            std::cout << "Successfully parsed \"" << filename << "\"" << std::endl;
        }
        else
        {
            std::cerr << "Failed to parse \"" << filename << "\"" << std::endl;
        }
    }

    return 0;
}
