#ifndef HEXVIEWTAB_H
#define HEXVIEWTAB_H

#include "QHexView/qhexview.h"
#include "core/ToolTab.h"
#include <QWidget>
#include <qfileinfo.h>

class HexViewTab : public ToolTab
{
    Q_OBJECT

private:

    /**
     * @brief Виджет HexView
    */
    QHexView* m_hexViewWidget;

    /**
     * @brief Создаёт страницу для вкладки формата
    */
    QWidget* createPage();

public:
    explicit HexViewTab(QWidget *parent = nullptr);

    QString toolName() const override { return "HEX"; };
    QIcon toolIcon() const override { return QIcon(":/icons/hex.png"); };

public slots:

    // From Parrent Class: ToolTab
    void setFile(QString filepath) override;
    void setTabData() override;
    void saveTabData() override;

};

#endif // HEXVIEWTAB_H
