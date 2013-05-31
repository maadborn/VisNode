/*
 * nodeview.h
 *
 * NodeView is the View part of Qt's MVC pattern, here subclassed from QAbstractItemView
 * to work with NodeItemModel and NodeItemDelegate.
 *
 * Mats Adborn, 2013-05-12
 */

#ifndef NODEVIEW_H
#define NODEVIEW_H

#include <QAbstractItemView>
#include <QWidget>
#include <QObject>

class QSize;

class NodeView : public QAbstractItemView
{
    Q_OBJECT
public:
    explicit NodeView(const QSize& modelTargetSize, QWidget *parent = 0);

    QRect visualRect(const QModelIndex &index) const;
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
    QModelIndex indexAt(const QPoint &point) const;
      
public slots:
    
protected:
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    int horizontalOffset() const;
    int verticalOffset() const;

    bool isIndexHidden(const QModelIndex &index) const;

    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
    QRegion visualRegionForSelection(const QItemSelection &selection) const;

    void paintEvent(QPaintEvent *);

    void resizeEvent(QResizeEvent *event);
    void updateGeometries();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QRect rectForRow(int row) const;
    QRect viewportRectForRow(int row) const;

    void paintConnections(const QModelIndex &index, QPainter* painter) const;
    void drawArrowToEdge(QPainter* painter, const QPoint& start, const QPoint& end) const;

    int modelTargetWidth;
    int modelTargetHeight;

    bool mouseLBDown;
    QPoint mouseLBDownOrigin;
    int mouseLBDownHScrollOrigin;
    int mouseLBDownVScrollOrigin;
    QModelIndex dragIndex;
};

#endif // NODEVIEW_H
