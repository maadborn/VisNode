#include "nodecreator.h"

/*
 *  Constructor
 */
NodeCreator::NodeCreator(QList<NodeItem*>& nodelist, QObject* nodeObjectParent)
    : _nodelist(nodelist), _nodeObjectParent(nodeObjectParent)
{
}


/*
 *  Checks and does:
 *      The parent node exists (parentName)             - if not: creates it
 *      The current node exists (nodeName)              - if not: creates it
 *      The parent node has the current node as child   - if not: adds it
 */
void NodeCreator::createNode(const QString& nodeName, const QString& parentName, const QColor& color, const QColor &parentColor)
{
    NodeItem* currentNode;
    NodeItem* parentNode;

    if (!parentColor.isValid()) {
        const_cast<QColor&>(parentColor) = color;
    }

    // If the parent node doesn't exist, create it. Otherwise, have it returned.
    if ((parentNode = getNode(parentName)) == NULL) {
        parentNode = addNode(nodeName, parentColor);
    }

    // If the current node doesn't exist, create it. Otherwise, have it returned.
    if ((currentNode = getNode(nodeName)) == NULL) {
        currentNode = addNode(nodeName, color);
    }

    // If the parent node doesn't have the current node as a child, add it.
    if (!parentNode->hasChild(nodeName)) {
        parentNode->addChild(currentNode);
    }
}

/*
 *  Creates a node (unless it already exists) without adding it to any parent/child relationship
 */
void NodeCreator::createStandAloneNode(const QString &nodeName, const QColor &color)
{
    NodeItem* currentNode;

    // If the current node doesn't exist, create it. Otherwise, have it returned.
    if ((currentNode = getNode(nodeName)) == NULL) {
        currentNode = addNode(nodeName, color);
    }
}

/*
 *  Utility function that adds a node to the node list
 */
NodeItem* NodeCreator::addNode(const QString& name, const QColor& color)
{
    NodeItem* newNode = new NodeItem(_nodelist.size(), name, color, _nodeObjectParent);
    _nodelist.append(newNode);

    return newNode;
}


/*
 *  Utility function that sets the parent QObject for all the nodes.
 *  Setting a parent ensures they are deleted with the parent.
 */
void NodeCreator::setNodeObjectParent(QObject* parent)
{
    _nodeObjectParent = parent;
}


/*
 *  Returns the node with the supplied name.
 *  If if doesn't exist, NULL is returned.
 */
NodeItem* NodeCreator::getNode(const QString &nodeName) const
{
    for (int i = 0; i < _nodelist.size(); ++i) {
        if (_nodelist[i]->name() == nodeName)
            return _nodelist[i];
    }
    return NULL;
}

/* Some doodling...

För xml:
    Är i:           child   1
    Känner till:    parent  1

För C++ källkod:
    Är i:           parent  1
    Känner till:    child   n

Slutsats: låt parent lägga till children, i createNode()

*/
