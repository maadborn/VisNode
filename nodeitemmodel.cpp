#include "nodeitemmodel.h"

/*
 *  Constructor
 */
NodeItemModel::NodeItemModel(QList<NodeItem*>& nodelist, QObject* parent)
    : QAbstractListModel(parent), _nodelist(nodelist), _posCalc(new DistrShapePositionCalc(nodelist))
{
}

NodeItemModel::~NodeItemModel()
{
    delete _posCalc;
}

/*
 *  Returns the total number of nodes
 */
int NodeItemModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);           // Suppress compiler warnings of the variable being unused
    return _nodelist.size();
}

/*
 *  Returns the correct data item as a QVariant, specified by index and role
 */
QVariant NodeItemModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() && index.row() < _nodelist.size())
        return QVariant();

    if (role == Qt::DisplayRole)
        return _nodelist.at(index.row())->data(NodeItem::NameRole);

    if (role == NodeItem::PositionRole)
        return _nodelist.at(index.row())->data(role);

    if (role == NodeItem::NumChildrenRole)
        return _nodelist.at(index.row())->data(role);

    if (role == NodeItem::ColorRole)
        return _nodelist.at(index.row())->data(role);

    if (role == NodeItem::ChildrenRole) {               // This is NOT optimal code, but I couldn't get it to work with the Meta Object system
        NodeItem* node = _nodelist.at(index.row());
        QList<QVariant> pointslist;

        foreach (NodeItem* child, node->children()) {
            pointslist << QVariant(child->position());
        }

        QVariant var = QVariant::fromValue(pointslist);

        return var;
    }

    return QVariant();
}

bool NodeItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() && index.row() < _nodelist.size())
        return false;

    if (role == NodeItem::PositionRole) {
        _nodelist.at(index.row())->setPosition(value.toPoint());
        return true;
    }

    return false;
}

/*
 *  Implementation of abstract function form base class
 *  Returns a dummy value if the model should be used in another view than NodeView
 */
QVariant NodeItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || role != NodeItem::PositionRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Col %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

/*
 *  Asks the NodeItemPositionCalculator to recalculate the node positions
 */
void NodeItemModel::recalculateNodePositions()
{
    _posCalc->calculate();
}

/*
 *  Returns the geometric size of the painted nodes
 */
const QSize& NodeItemModel::modelGeometricSize() const
{
    return _posCalc->modelGeometricSize();
}

/*
 *  Scales the painted nodes to a desired geometric size
 */
void NodeItemModel::scaleNodePositions(const QSize &sizeToFit)
{
    _posCalc->scaleTo(sizeToFit);
}

/*
 *  Moves the painted nodes to center around a new point
 */
void NodeItemModel::moveNodePositions(const QPoint &newCenterPoint)
{
    _posCalc->moveInto(newCenterPoint);
}
