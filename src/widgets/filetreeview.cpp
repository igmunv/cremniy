#include "filetreeview.h"


FileTreeView::FileTreeView(QWidget *parent)
    : QTreeView(parent)
{
    // Drag & Drop
    this->setDragEnabled(true);
    this->setAcceptDrops(true);
    this->setDropIndicatorShown(true);
    this->setDragDropMode(QAbstractItemView::InternalMove);

    m_expandTimer = new QTimer(this);
    m_expandTimer->setSingleShot(true);
    m_expandTimer->setInterval(600);

    connect(m_expandTimer, &QTimer::timeout, this, [this](){
        if (m_hoverIndex.isValid())
            this->expand(m_hoverIndex);
    });
}

void FileTreeView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex index = this->indexAt(event->pos());
    if (index.isValid())
        emit mouseClicked(index, event->button());

    QTreeView::mousePressEvent(event);
}
