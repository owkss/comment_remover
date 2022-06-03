#include "traverser.h"
#include "clang-c/Index.h"

#include <iostream>

#include <QFile>
#include <QDebug>
#include <QBuffer>
#include <QVector>
#include <QByteArray>

typedef struct Range
{
    unsigned int begin_offset;
    unsigned int end_offset;
    unsigned int len_offset;
} Range;

static int iterate_ranges(const char *filename, const QVector<Range> &ranges)
{
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly))
    {
        std::cerr << "open() failed" << std::endl;
        return 5;
    }

    QByteArray data = f.readAll();
    f.close();

    QByteArray result;
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);

    for (int i = 0; i < ranges.size(); ++i)
    {
        const Range &r = ranges.at(i);

        result += buffer.read(r.begin_offset - buffer.pos());
        buffer.seek(r.end_offset);
    }

    result += buffer.read(data.size() - result.size());

    f.open(QIODevice::WriteOnly);
    qint64 bytes = f.write(result);

    return ((bytes == result.size()) ? 0 : 6);
}

std::ostream &operator<<(std::ostream &stream, const CXString &str)
{
    stream << clang_getCString(str);
    clang_disposeString(str);
    return stream;
}

static CXChildVisitResult visit(CXCursor c, CXCursor parent, CXClientData client_data)
{
    CXSourceLocation loc = clang_getCursorLocation(c);
    if (!clang_Location_isFromMainFile(loc))
        return CXChildVisit_Continue;

    CXCursorKind kind = clang_getCursorKind(c);
    switch (kind)
    {
    case CXCursor_CompoundStmt: /* Участок между фигурными скобками */
        if (clang_getCursorKind(parent) == CXCursor_FunctionDecl) /* Внутри функции */
        {
            CXTranslationUnit unit = clang_Cursor_getTranslationUnit(c);
            CXSourceRange range = clang_getCursorExtent(c);

            CXToken *tokens = nullptr;
            unsigned int num_tokens = 0;
            clang_tokenize(unit, range, &tokens, &num_tokens);

            for (unsigned int i = 0; i < num_tokens; ++i)
            {
                CXToken t = tokens[i];
                CXTokenKind kind = clang_getTokenKind(t);

                if (kind == CXToken_Comment)
                {
                    //std::cout << clang_getTokenSpelling(unit, t) << std::endl;

                    CXSourceRange token_range = clang_getTokenExtent(unit, t);
                    CXSourceLocation begin = clang_getRangeStart(token_range);
                    CXSourceLocation end = clang_getRangeEnd(token_range);

                    CXFile file;
                    unsigned int begin_line, begin_column, begin_offset;
                    unsigned int end_line, end_column, end_offset;

                    clang_getSpellingLocation(begin, &file, &begin_line, &begin_column, &begin_offset);
                    clang_getSpellingLocation(end, &file, &end_line, &end_column, &end_offset);

                    Range r;
                    r.begin_offset = begin_offset;
                    r.end_offset = end_offset;
                    r.len_offset = end_offset - begin_offset;

                    QVector<Range> *ranges = static_cast<QVector<Range>*>(client_data);
                    ranges->push_back(r);
                }
            }

            clang_disposeTokens(unit, tokens, num_tokens);
        }
        break;
    default:
        break;
    }

    return CXChildVisit_Recurse;
}

int traverser::parse(const char *filename)
{
    const char *args[] = { "-fparse-all-comments" };

    CXIndex index = clang_createIndex(0, 0);

    CXTranslationUnit unit = nullptr;
    CXErrorCode code = clang_parseTranslationUnit2(index, filename,
                                                   args, sizeof(args) / sizeof(args[0]),
                                                   nullptr, 0,
                                                   CXTranslationUnit_DetailedPreprocessingRecord, &unit);

    switch (code)
    {
    case CXError_Success:
        std::cout << "CXError_Success" << std::endl;
        break;
    case CXError_Failure:
        std::cerr << "CXError_Failure" << std::endl;
        return 1;
    case CXError_Crashed:
        std::cerr << "CXError_Crashed" << std::endl;
        return 2;
    case CXError_InvalidArguments:
        std::cerr << "CXError_InvalidArguments" << std::endl;
        return 3;
    case CXError_ASTReadError:
        std::cerr << "CXError_ASTReadError" << std::endl;
        return 4;
    }

    QVector<Range> ranges;

    CXCursor cursor = clang_getTranslationUnitCursor(unit);
    clang_visitChildren(cursor, visit, &ranges);

    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);

    return iterate_ranges(filename, ranges);
}
