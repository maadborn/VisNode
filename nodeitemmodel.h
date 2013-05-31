/*
 * nodeitemmodel.h
 *
 * NodeItemModel is part of Qt's MVC pattern, representing the Model.
 * It contains the data (nodes) and responds to calls from the View and Delegate.
 *
 * Mats Adborn, 2013-05-12
 */

#ifndef NODEITEMMODEL_H
#define NODEITEMMODEL_H

#include "nodeitem.h"
#include "abstractnodeitempositioncalc.h"
#include "circleshapepositioncalc.h"
#include "distrshapepositioncalc.h"
#include <QAbstractListModel>
#include <QList>


class QSize;

class NodeItemModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit NodeItemModel(QList<NodeItem*>& nodelist, QObject* parent = 0);
    ~NodeItemModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
      
    void recalculateNodePositions();
    const QSize& modelGeometricSize() const;
    void scaleNodePositions(const QSize& sizeToFit);
    void moveNodePositions(const QPoint& newCenterPoint);

private:
    QList<NodeItem*>& _nodelist;
    AbstractNodeItemPositionCalc* _posCalc;
};


#endif // NODEITEMMODEL_H
