#include "nodeitem.h"


/*
 *  Constructor
 */
NodeItem::NodeItem(int index, const QString& name, const QColor& color, QObject* parent)
    : QObject(parent), _row(index), _name(name), _color(color)
{
}

NodeItem::NodeItem(const NodeItem &other)
    : QObject(0)
{
    _row = other._row;
    _name = other._name;
    _position = other._position;
    _data = other._data;
    _children = other._children;
    _color = other._color;
}

/*
 *  Destructor
 */
NodeItem::~NodeItem()
{
    //qDeleteAll(_children);
}


/*
 *  Returns the name of the node
 */
QString NodeItem::name() const
{
    return _name;
}


/*
 *  Returns node data using one of its roles
 */
QVariant NodeItem::data(int role) const
{
    switch (role)
    {
        case (NameRole):
            return name();

        case (PositionRole):
            return position();

        case (NumChildrenRole):
            return childCount();

        case (ColorRole):
            return color();

        default:
            return QVariant();
    }
}


/*
 *  Returns the [row]th child of the node
 */
NodeItem* NodeItem::child(int row) const
{
    return dynamic_cast<NodeItem*>(_children.value(row));
}


/*
 *  Returns the list of children to the node
 */
QList<NodeItem*> &NodeItem::children()
{
    return _children;
}


/*
 *  Returns the position of the node
 */
QPoint NodeItem::position() const
{
    return _position;
}


/*
 *  Returns the number of children
 */
int NodeItem::childCount() const
{
    return _children.size();
}


/*
 *  Returns number of data columns the node has
 */
int NodeItem::columnCount() const
{
    return _data.size();
}


/*
 *  Returns the node's position in the node list
 */
int NodeItem::row() const
{
    return _row;
}


/*
 *  Returns the node's color
 */
QColor NodeItem::color() const
{
    return _color;
}


/*
 *  Sets the color of the node
 */
void NodeItem::setColor(const QColor& color)
{
    _color = color;
}


/*
 *  Sets the position of the node
 */
void NodeItem::setPosition(const QPoint& position)
{
    _position = position;
}


/*
 *  Adds the child to the node
 */
void NodeItem::addChild(NodeItem *child)
{
    _children.append(child);
}


/*
 *  Verifies if the node has a child with the supplied name.
 *  Returns true if that's the case, otherwise false.
 */
bool NodeItem::hasChild(const QString &nameOfChildNode) const
{
    for (int i = 0; i < _children.size(); ++i) {
        if (dynamic_cast<NodeItem*>(_children.at(i))->name() == nameOfChildNode)
            return true;
    }
    return false;
}


///*
// *  Returns the custom role names the node has
// */
//QHash<int, QByteArray> NodeItem::roleNames() const
//{
//    QHash<int, QByteArray> roles;
//    roles[NameRole] = "name";
//    roles[PositionRole] = "position";
//    return roles;
//}


/*
 *  Overloaded operator==
 */
bool NodeItem::operator==(const NodeItem &other) const
{
    if (_name == other.name())
        return true;
    else
        return false;
}



