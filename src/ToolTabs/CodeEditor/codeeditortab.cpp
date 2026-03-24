#include "codeeditortab.h"
#include "QCodeEditor.hpp"
#include <qboxlayout.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstackedlayout.h>
#include "filemanager.h"
#include "utils.h"

#include "core/ToolTabFactory.h"

static bool registered = [](){
    ToolTabFactory::instance().registerTab("1", [](){
        return new CodeEditorTab();
    });
    return true;
}();

CodeEditorTab::CodeEditorTab(QWidget *parent)
    : ToolTab{parent}
{

    // - - Create "Code Editor" Page - -

    m_codeEditorWidget = new QCodeEditor(this);

    QTextOption opt = m_codeEditorWidget->document()->defaultTextOption();
    opt.setTabStopDistance(20);
    m_codeEditorWidget->document()->setDefaultTextOption(opt);

    m_codeEditorWidget->document()->markContentsDirty(0, m_codeEditorWidget->document()->characterCount());
    m_codeEditorWidget->viewport()->update();

    // - - Create "Binary File Detected" Page - -

    m_overlayWidget = new QWidget(this);

    auto overlayLayout = new QVBoxLayout(m_overlayWidget);
    overlayLayout->setAlignment(Qt::AlignCenter);

    QLabel* title = new QLabel("Binary file detected");
    title->setStyleSheet("color: white; font-size: 20px;");
    title->setAlignment(Qt::AlignCenter);
    title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    overlayLayout->addWidget(title);
    overlayLayout->addSpacing(15);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setAlignment(Qt::AlignCenter);

    QPushButton* anywayOpenBtn = new QPushButton("Open anyway");

    btnLayout->addWidget(anywayOpenBtn);
    overlayLayout->addLayout(btnLayout);

    // - - Create and Init Stacked Layout Widget - -

    auto stack = new QStackedLayout;
    stack->setStackingMode(QStackedLayout::StackAll);
    stack->addWidget(m_codeEditorWidget);
    stack->addWidget(m_overlayWidget);

    m_overlayWidget->hide();

    this->setLayout(stack);

    // - - Connects - -

    // Trigger: Menu Bar: View->wordWrap - setWordWrapMode
    // connect(GlobalWidgetsManager::instance().get_IDEWindow_menuBar_view_wordWrap(),
    //         &QAction::changed,
    //         this, [this]{
    //             if (GlobalWidgetsManager::instance().get_IDEWindow_menuBar_view_wordWrap()->isChecked())
    //                 m_codeEditorWidget->setWordWrapMode(QTextOption::WordWrap);
    //             else
    //                 m_codeEditorWidget->setWordWrapMode(QTextOption::NoWrap);
    //         });

    // Clicked: Open File Anyway Button
    connect(anywayOpenBtn, &QPushButton::clicked, this, [this]{
        forceSetData = true;
        this->setTabData();
    });

    // modificationChanged: signal modifyData
    connect(m_codeEditorWidget->document(),
            &QTextDocument::modificationChanged,
            this,
            [this](bool modified){
                if (!m_codeEditorWidget->m_ignoreModification)
                    setModifyIndicator(true);
                    emit modifyData();
            });

    // ContentsChanged: if new hash == old hash: dataEqual, else: signal modifyData
    connect(m_codeEditorWidget->document(),
            &QTextDocument::contentsChanged,
            this,
            [this](){
                QByteArray data = m_codeEditorWidget->getBData();
                uint newDataHash = qHash(data, 0);
                if (m_dataHash == newDataHash) {
                    setModifyIndicator(false);
                    emit dataEqual();
                }
                else{
                    if (!m_codeEditorWidget->m_ignoreModification)
                        setModifyIndicator(true);
                        emit modifyData();
                }
            });

}

// - - override functions - -

// - public slots -

void CodeEditorTab::setFile(QString filepath){
    m_fileContext = new FileContext(filepath);
    QFileInfo fileInfo(filepath);
    QString ext = (fileInfo.suffix()).toLower();
    m_codeEditorWidget->setFileExt(ext);
}

void CodeEditorTab::setTabData(){

    qDebug() << "CodeEditorTab: setTabData";

    QByteArray data = FileManager::openFile(m_fileContext);

    if (isBinary(data) && !forceSetData){
        m_codeEditorWidget->hide();
        m_overlayWidget->show();
    }
    else{
        m_dataHash = qHash(data, 0);
        m_codeEditorWidget->show();
        m_overlayWidget->hide();
        m_codeEditorWidget->setBData(data);
        forceSetData = false;
    }

    setModifyIndicator(false);
    emit dataEqual();
}

void CodeEditorTab::saveTabData() {
    qDebug() << "CodeEditorTab: saveTabData";

    QByteArray data = m_codeEditorWidget->getBData();
    uint newDataHash = qHash(data, 0);
    if (newDataHash == m_dataHash) return;
    m_dataHash = newDataHash;

    FileManager::saveFile(m_fileContext, &data);

    m_codeEditorWidget->document()->setModified(false);

    setModifyIndicator(false);
    emit dataEqual();
    emit refreshDataAllTabsSignal();
}