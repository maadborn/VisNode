#include "cppnodeparser.h"
#include <QRegularExpression>
#include <QFile>
#include <QStringRef>
#include <QDir>
#include <QColor>

static const QColor STANDARD_COLOR(140,200,240);    // A light blue color
static const QColor CUSTOM_COLOR(180,255,150);      // A light green color

/*
 *  Constructor
 */
CPPNodeParser::CPPNodeParser(QList<NodeItem*>& nodeList)
    : AbstractNodeParser(nodeList)
{
}

/*
 *  Destructor
 */
CPPNodeParser::~CPPNodeParser()
{
}

/*
 *  Processes the file, looking at lines starting with #include,
 *  creating nodes from the those classes when such lines are found.
 *  Currently just returns true...
 */
bool CPPNodeParser::processFile(QFile& file)
{
    // The path separator character, translated to native symbol, excludes comments
    QRegularExpression rxNativeSlash(QString("[^/]") + QDir::toNativeSeparators("/") + QString("{1}[^/\\*\\s]"));

    QRegularExpression rxInclude("^#include");          // The word 'include' is at the beginning of the line
    QRegularExpression rxStartName("[<\"]");            // The characters '<' or '"' ("path container" start)
    QRegularExpression rxEndName("[>\"]");              // The characters '>' or '"' ("path container" end)

    // Setup the file to stream from
    stream.setDevice(&file);

    QString fileName = file.fileName();

    // Look for the folder separator symbol in the filename (path)
    int fileSlashIndex = fileName.lastIndexOf(rxNativeSlash);

    // If the fileName (incl. path) contains a folder separator symbol, create a substring with just the filename and extension
    if (fileSlashIndex != -1)
        fileName = QStringRef(&fileName, fileSlashIndex+2, fileName.length()-fileSlashIndex-2).toString();

    // Looks like it isn't needed anymore, but I'll keep these lines until later...
    // Create a node of the current file
    if (nodeCreator()->getNode(fileName) == NULL)
        nodeCreator()->createStandAloneNode(fileName, CUSTOM_COLOR);

    QString line;
    QStringRef foundName;
    QColor nodeColor;

    // Read all lines in the stream
    while (!stream.atEnd()) {
        line = stream.readLine();
        line = line.trimmed();                          // Remove all spaces at the beginning and the end of the line

        // If the line starts with 'include'
        if (rxInclude.match(line).hasMatch()) {
            // If it's a non-standard class (ought to be if it's path is set with ""), give it a custom color
            if (line.contains('"'))
                nodeColor = CUSTOM_COLOR;
            else
                nodeColor = STANDARD_COLOR;

            int startIndex = line.indexOf(rxStartName);         // Get index of the "path container's" start character
            int endIndex = line.lastIndexOf(rxEndName);         // Get index of the "path container's" end character
            int slashIndex = line.lastIndexOf(rxNativeSlash);   // Get index of any '/' character (e.g. <sys/time.h>)

            // If a slash character was found, replace startIndex
            if (slashIndex != -1)
                startIndex = slashIndex + 1;

            startIndex += 1;                            // We want to start at the character just after startIndex

            foundName = QStringRef(&line, startIndex, endIndex-startIndex);     // Get the substring with the name of the file/unit

            _nodeCreator->createNode(foundName.toString(), fileName, nodeColor, CUSTOM_COLOR); // Create the node!
        }
    }

    stream.resetStatus();

    return true;
}
