#include "xmlnodeparser.h"
#include <QFile>
#include <QDebug>

static const QColor START_COLOR(153, 238, 99);      // A light-green color
static const int COLOR_CHANGE_STEP = 30;

/*
 *  Constructor
 */
XMLNodeParser::XMLNodeParser(QList<NodeItem*>& nodeList)
    : AbstractNodeParser(nodeList)
{
}


/*
 *  Destructor
 */
XMLNodeParser::~XMLNodeParser()
{
}


/*
 *  Processes the file, going through its 'tokens',
 *  creating nodes when new ones are found.
 *  Currently just returns true...
 */
bool XMLNodeParser::processFile(QFile& file)
{
    _reader.setDevice(&file);

    _currentColor = START_COLOR;

    nextNode();

    return true;
}


/*
 *  Recursive function that checks for the next start element.
 *  When such is found, it tries to create a node and then calls itself again,
 *  using the node just found as the current parent.
 *  Continues until the end of the document.
 */
void XMLNodeParser::nextNode(QString parentName)
{
    QString currentName;

    do {
        _reader.readNext();                                     // Make sure we move (at least) one additional token forward

        while (!(_reader.isStartElement() || _reader.isEndElement()))   // We're only interested in start and end elements
            _reader.readNext();

        currentName = _reader.name().toString();                // Save the name of the current element to separate it from
                                                                // _reader, and for ease of use of the string

        if (_reader.isStartElement()) {                         // Is the reader at a start element?

            //qDebug() << "name:" << currentName << "_currentColor:" << _currentColor;

            _nodeCreator->createNode(currentName, parentName, _currentColor);

            //qDebug() << "<" << qPrintable(currentName) << " (parent = " << qPrintable(parentName) << ")>";

            changeHsvHue(COLOR_CHANGE_STEP);                   // Modify color before going further recursively

            nextNode(currentName);                              // Recursive call on this function for the new level

            changeHsvHue(-COLOR_CHANGE_STEP);                    // Restore color after coming back from the recursive call
        }

    } while (currentName != parentName && !parentName.isEmpty() && !_reader.atEnd());
    // The while-conditions work by checking:
    // 1) If the closing tag of the current started one is found
    // 2) If the "above root" (=no name, doesn't exist) element is found
    // 3) If the reader is at the end
    // Testing showed all three were needed to parse successfully
}

/*
 *  Prints the XML hierarchy (startElement and stopElement only) with indentation.
 *  Mainly a debugging feature to verify the structure of what we are interested
 *  to find when creating the nodes.
 *  Used in the same way as nextNode(), function calls could be used more or less interchangeably.
 */
void XMLNodeParser::printNodeRecursive(QString parentName, int indent)
{
    QString currentName;
    bool endSection = false;                                    // Set to true if we enter a subsection with 1+ start elements

    do {
        _reader.readNext();                                     // Make sure we move (at least) one additional token forward

        while (!(_reader.isStartElement() || _reader.isEndElement()))   // We're only interested in start and end elements
            _reader.readNext();

        currentName = _reader.name().toString();                // Save the name of the current element to separate it from
                                                                // _reader, and for ease of use of the string
        if (_reader.isStartElement()) {                         // START ELEMENT?
            endSection = true;                                  // This section will need to be closed later (see else clause)

            std::cout << std::endl;                             // Break the line...

            for (int i = 0; i < indent; i++)                    // ... and indent to correct level
                std::cout << "  ";

            std::cout << "<" << qPrintable(currentName) << ">";     // Print the start tag

            indent++;                                           // Increase the indent for the next element
            printNodeRecursive(currentName, indent);            // Recursive call on this function for the new level
            indent--;                                           // When returning, decrease indent to prior level
        }
        else {                                                  // NOT START ELEMENT == STOP ELEMENT
            if (endSection) {                                   // If this instance of the function (not a recursive one)
                std::cout << std::endl;                         // has been run and found a startElement, it needs to be
                for (int i = 0; i < (indent-1); i++)            // "closed" in a good looking way. This ensures correct
                    std::cout << "  ";                          // linebreak and indentation.
            }

            std::cout << "</" << qPrintable(currentName) << ">";    // Print the end tag
        }

    } while (currentName != parentName && !_reader.atEnd() && parentName != "null");
}

/*
 *  Change the hue value of the next parsed node
 */
void XMLNodeParser::changeHsvHue(int increaseHueWith)
{
    int h, s, v, a;
    _currentColor.getHsv(&h, &s, &v, &a);
//    qDebug() << h << s << v << a;
    if (h + increaseHueWith >= 0 && h + increaseHueWith <= 255)
        h += increaseHueWith;

    _currentColor.setHsv(h, s, v, a);
}


