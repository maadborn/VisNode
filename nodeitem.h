/*
 * nodeitem.h
 *
 * NodeItem describes and represents the nodes, which could be xml-elements or files in other parsemodes
 *
 * Mats Adborn, 2013-05-12
 */


#ifndef NODEITEM_H
#define NODEITEM_H

#include <QMetaType>
#include <QObject>
#include <QString>
#include <QHash>
#include <QVariant>
#include <QPoint>
#include <QColor>


class NodeItem : public QObject
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PositionRole,
        NumChildrenRole,
        ChildrenRole,
        ColorRole
    };

    NodeItem() : QObject(0) {}
    NodeItem(const NodeItem&);
    explicit NodeItem(int index, const QString& name, const QColor& color = QColor(), QObject* parent = 0);

    ~NodeItem();

    QString name() const;
    QVariant data(int role) const;

    NodeItem* child(int row) const;
    QList<NodeItem*>& children();
    void addChild(NodeItem* child);
    bool hasChild(const QString& nameOfChildNode) const;

    int childCount() const;
    int columnCount() const;

    int row() const;

    QColor color() const;
    void setColor(const QColor& color);

    QPoint position() const;
    void setPosition(const QPoint& position);

//    QHash<int, QByteArray> roleNames() const;     // Unused

    bool operator==(const NodeItem& other) const;

private:
    int _row;
    QString _name;
    QPoint _position;
    QList<QVariant> _data;          // Unused...
    QList<NodeItem*> _children;
    QColor _color;
};


// Register the type in Qt's Meta-Object system, for use in QVariant
// Work in progress, used workaround instead for the time being
//Q_DECLARE_METATYPE(NodeItem)
//Q_DECLARE_OPAQUE_POINTER(NodeItem)



#endif // NODEITEM_H
