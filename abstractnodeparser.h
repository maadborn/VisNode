/*
 * abstractnodeparser.h
 *
 * Abstract class for the file parsers
 *
 * Mats Adborn, 2013-05-01
 */


#ifndef ABSTRACTNODEPARSER_H
#define ABSTRACTNODEPARSER_H

#include "nodecreator.h"
#include <iostream>
#include <QList>

class QFile;
class QString;
class QStringList;
class NodeItem;

class AbstractNodeParser
{
public:
    AbstractNodeParser(QList<NodeItem*>& nodeList);
    virtual ~AbstractNodeParser();
    bool parseFile(const QString& fileName);
    bool parseFiles(const QStringList& fileNames);

    NodeCreator* nodeCreator() const;

protected:
    virtual bool processFile(QFile& file) = 0;

    QList<NodeItem*>& _nodelist;
    NodeCreator* _nodeCreator;
};

#endif // ABSTRACTNODEPARSER_H
