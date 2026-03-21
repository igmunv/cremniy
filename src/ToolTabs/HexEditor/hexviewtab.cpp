#include "hexviewtab.h"
#include "verticaltabstyle.h"
#include <qapplication.h>
#include <qboxlayout.h>
#include <qstackedwidget.h>
#include <qtabwidget.h>
#include <QListWidget>
#include <QTableWidget>
#include "filemanager.h"
#include "core/ToolTabFactory.h"

static bool registered = [](){
    ToolTabFactory::instance().registerTab("HexViewTab", [](){
        return new HexViewTab();
    });
    return true;
}();

HexViewTab::HexViewTab(QWidget *parent)
    : ToolTab{parent}
{
    // - - Tab Widgets - -

    // Create Layout
    auto mainHexTabLayout = new QHBoxLayout(this);
    mainHexTabLayout->setSpacing(0);
    mainHexTabLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainHexTabLayout);

    // Create Tab Widgets
    QListWidget* tabsList = new QListWidget();
    tabsList->setObjectName("hexTabsList");
    tabsList->setFocusPolicy(Qt::NoFocus);
    QStackedWidget* tabView = new QStackedWidget();

    // Add TabWidgets in Layout
    mainHexTabLayout->addWidget(tabView);
    mainHexTabLayout->addWidget(tabsList);

    // Add Tab Names in TabsList
    tabsList->addItem("Raw");
    tabsList->addItem("ELF");
    tabsList->addItem("PE");
    tabsList->addItem("MBR");

    // - - Create Pages - -

   // RAW page
    m_hexViewWidget = new QHexView(this);
    auto pageRaw = createPage();
    pageRaw->layout()->addWidget(m_hexViewWidget);

    // ELF page
    auto pageELF = createPage();

    // PE page
    auto pagePE = createPage();

    // MBR page
    auto pageMBR = createPage();

    // - - End Configurate Tab Widgets - -

    // Add Pages in TabView
    tabView->addWidget(pageRaw);
    tabView->addWidget(pageELF);
    tabView->addWidget(pagePE);
    tabView->addWidget(pageMBR);

    // Configurate
    tabsList->setCurrentRow(0);

    // - - Connects - -

    // TabList: select tab
    connect(tabsList, &QListWidget::currentRowChanged,
                     tabView, &QStackedWidget::setCurrentIndex);

    // hexViewWidget: data change
    connect(m_hexViewWidget->hexDocument(),
            &QHexDocument::changed,
            this,
            [this](){

                QByteArray data = m_hexViewWidget->getBData();
                uint newDataHash = qHash(data, 0);
                if (m_dataHash == newDataHash) {
                    emit dataEqual();
                }
                else{
                    if (!m_hexViewWidget->m_ignoreModification)
                        emit modifyData(true);
                }
            });
}

// Create default page
QWidget* HexViewTab::createPage(){
    QWidget* pageWidget = new QWidget();
    QVBoxLayout* pageWidgetLayout = new QVBoxLayout(pageWidget);
    pageWidgetLayout->setContentsMargins(0,0,0,0);
    pageWidget->setLayout(pageWidgetLayout);
    return pageWidget;
}


// - - override functions - -

// - public slots -

void HexViewTab::setFile(QString filepath){
    m_fileContext = new FileContext(filepath);
}

void HexViewTab::setTabData(){
    qDebug() << "HexViewTab: setTabData()";

    QByteArray data = FileManager::openFile(m_fileContext);

    m_dataHash = qHash(data, 0);
    m_hexViewWidget->setBData(data);
    emit dataEqual();
};

void HexViewTab::saveTabData() {
    qDebug() << "HexViewTab: saveTabData";

    QByteArray data = m_hexViewWidget->getBData();
    uint newDataHash = qHash(data, 0);
    if (newDataHash == m_dataHash) return;
    m_dataHash = newDataHash;

    FileManager::saveFile(m_fileContext, &data);

    emit dataEqual();
    emit refreshDataAllTabsSignal();
}