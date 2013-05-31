/*
 * xmlnodeparser.h
 *
 * XMLNodeParser is a concrete subclass of the AbstractNodeParser, focusing on XML files.
 *
 * Mats Adborn, 2013-05-12
 */

#ifndef XMLNODEPARSER_H
#define XMLNODEPARSER_H

#include "abstractnodeparser.h"
#include <iostream>
#include <QList>
#include <QXmlStreamReader>

class QFile;
class NodeItem;

class XMLNodeParser : public AbstractNodeParser
{
public:
    XMLNodeParser(QList<NodeItem*>& nodeList);
    virtual ~XMLNodeParser();

protected:
    bool processFile(QFile& file);
    void nextNode(QString parentName = QString());

private:
    QXmlStreamReader _reader;
    QColor _currentColor;

    void printNodeRecursive(QString parentName, int indent = 0);
    void changeHsvHue(int increaseHueWith);
};

#endif // XMLNODEPARSER_H
