#include "abstractnodeparser.h"
#include <QFile>
#include <QStringList>


/*
 *  Constructor
 */
AbstractNodeParser::AbstractNodeParser(QList<NodeItem*>& nodeList)
    : _nodelist(nodeList)
{
    _nodeCreator = new NodeCreator(nodeList);
}


/*
 *  Destructor
 */
AbstractNodeParser::~AbstractNodeParser()
{
    delete _nodeCreator;
}


/*
 *  Opens a file and calls the subclass specific processFile() on it
 */
bool AbstractNodeParser::parseFile(const QString& fileName)
{    
    QFile file(fileName);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        std::cerr << "AbstractNodeParser::readFile(): Failed to open file " << qPrintable(fileName) << std::endl;
        return false;
    }

    if(!processFile(file)) {
        std::cerr << "AbstractNodeParser::readFile(): Failed to process file " << qPrintable(fileName) << std::endl;
        return false;
    }

    file.close();

    return true;
}


/*
 *  Convinient function for reading a list of file names.
 *  Returns true if all files were read without problem, otherwise false.
 */
bool AbstractNodeParser::parseFiles(const QStringList& fileNames)
{
    for (int i = 0; i < fileNames.size(); i++) {
        if (!parseFile(fileNames.at(i)))
            return false;
    }

    return true;
}


/*
 *  Returns the NodeCreator member
 */
NodeCreator* AbstractNodeParser::nodeCreator() const
{
    return _nodeCreator;
}


