/*
 * cppnodeparser.h
 *
 * CPPNodeParser is a concrete subclass of the AbstractNodeParser, focusing on C++ source files.
 *
 * Mats Adborn, 2013-05-13
 */


#ifndef CPPNODEPARSER_H
#define CPPNODEPARSER_H

#include "abstractnodeparser.h"
#include <QTextStream>
#include <QList>

class QFile;
class NodeItem;

class CPPNodeParser : public AbstractNodeParser
{
public:
    CPPNodeParser(QList<NodeItem*>& nodeList);
    virtual ~CPPNodeParser();

protected:
    bool processFile(QFile& file);

private:
    QTextStream stream;
};

#endif // CPPNODEPARSER_H
