#ifndef BINARYTAB_H
#define BINARYTAB_H

#include "core/ToolTab.h"
#include <QWidget>
#include <qfileinfo.h>
#include <qstackedwidget.h>

class BinaryTab : public ToolTab
{
    Q_OBJECT

private:

    QStackedWidget* pageView;

public:
    explicit BinaryTab(QWidget *parent = nullptr);

    QString toolName() const override { return "Binary"; };
    QIcon toolIcon() const override { return QIcon(":/icons/binary.png"); };

public slots:

    // From Parrent Class: ToolTab
    void setFile(QString filepath) override;
    void setTabData() override;
    void saveTabData() override;

    void pageModifyDataSlot();

};

#endif // BINARYTAB_H
