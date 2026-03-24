#include "binarytab.h"
#include "verticaltabstyle.h"
#include <qapplication.h>
#include <qboxlayout.h>
#include <qstackedwidget.h>
#include <qtabwidget.h>
#include <QListWidget>
#include <QTableWidget>
#include "filemanager.h"
#include "formatpagefactory.h"
#include "formatpage.h"
#include "core/ToolTabFactory.h"

static bool registered = [](){
    ToolTabFactory::instance().registerTab("2", [](){
        return new BinaryTab();
    });
    return true;
}();

BinaryTab::BinaryTab(QWidget *parent)
    : ToolTab{parent}
{
    // - - Tab Widgets - -

    // Create Layout
    auto mainHexTabLayout = new QHBoxLayout(this);
    mainHexTabLayout->setSpacing(0);
    mainHexTabLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainHexTabLayout);

    // Create Tab Widgets
    QListWidget* pageList = new QListWidget();
    pageList->setObjectName("hexTabsList");
    pageList->setFocusPolicy(Qt::NoFocus);
    pageView = new QStackedWidget();

    // Add TabWidgets in Layout
    mainHexTabLayout->addWidget(pageView);
    mainHexTabLayout->addWidget(pageList);

    // - - Create Pages - -
    auto& formatFactory = FormatPageFactory::instance();

    qDebug() << "FormatPageFactory constr: for id in avPages";
    for (const QString& toolID : formatFactory.availablePages()){
        FormatPage* fpage = formatFactory.create(toolID);
        qDebug() << "availablePage: " << fpage->pageName();

        if (fpage) {
            pageView->addWidget(fpage);
            pageList->addItem(fpage->pageName());

            connect(fpage, &FormatPage::modifyData, this, &BinaryTab::pageModifyDataSlot);
            connect(fpage, &FormatPage::dataEqual, this, &ToolTab::dataEqual);
        }
    }

    // - - End Configurate Tab Widgets - -

    // Configurate
    pageList->setCurrentRow(0);

    // - - Connects - -

    // TabList: select tab
    connect(pageList, &QListWidget::currentRowChanged,
                     pageView, &QStackedWidget::setCurrentIndex);
}


// - - override functions - -

// - public slots -

void BinaryTab::pageModifyDataSlot(){
    setModifyIndicator(true);
    emit modifyData();
}

void BinaryTab::setFile(QString filepath){
    m_fileContext = new FileContext(filepath);
}

void BinaryTab::setTabData(){
    qDebug() << "HexViewTab: setTabData(): start";

    QByteArray data = FileManager::openFile(m_fileContext);
    qDebug() << "HexViewTab: setTabData(): file opened";

    for (int pageIndex = 0; pageIndex < pageView->count(); pageIndex++){
        FormatPage* fpage = dynamic_cast<FormatPage*>(pageView->widget(pageIndex));
        qDebug() << "HexViewTab: setTabData(): start set page data for " << fpage->pageName();
        fpage->setPageData(data);
        qDebug() << "HexViewTab: setTabData(): success set page data for " << fpage->pageName();
    }

    m_dataHash = qHash(data, 0);
    setModifyIndicator(false);
    emit dataEqual();
    qDebug() << "HexViewTab: setTabData(): success";
};

void BinaryTab::saveTabData() {
    qDebug() << "HexViewTab: saveTabData";

    FormatPage* fpage = dynamic_cast<FormatPage*>(pageView->currentWidget());
    QByteArray data = fpage->getPageData();
    uint newDataHash = qHash(data, 0);
    if (newDataHash == m_dataHash) return;
    m_dataHash = newDataHash;

    FileManager::saveFile(m_fileContext, &data);
    setModifyIndicator(false);
    emit dataEqual();
    emit refreshDataAllTabsSignal();
}