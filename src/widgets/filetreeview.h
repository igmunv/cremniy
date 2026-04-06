#ifndef FILETREEVIEW_H
#define FILETREEVIEW_H

#include <QTreeView>
#include <QMouseEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QTimer>


class FileTreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit FileTreeView(QWidget *parent = nullptr);

signals:
    void mouseClicked(QModelIndex index, Qt::MouseButton button);

protected:
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QTimer* m_expandTimer;
    QModelIndex m_hoverIndex;
};


#endif // FILETREEVIEW_H
