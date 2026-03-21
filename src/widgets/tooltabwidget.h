#ifndef TOOLTABWIDGET_H
#define TOOLTABWIDGET_H

#include <QTabWidget>

class QVBoxLayout;
class QSyntaxStyle;
class QComboBox;
class QCheckBox;
class QSpinBox;
class QCompleter;
class QStyleSyntaxHighlighter;
class QCodeEditor;

class ToolTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    ToolTabWidget(QWidget *parent, QString path);
    int saveToFileCurrentTab(QString path);
    void setDataInTabs(QByteArray &data, int index = -1, int excluded_index = -1);

private:
    void loadStyle(QString path, QString name);

public slots:
    void saveCurrentTabData();
    void refreshDataAllTabs();

    void removeStar();
    void setupStar(bool modified);

signals:
    void removeStarSignal();
    void setupStarSignal();

};

#endif // TOOLTABWIDGET_H
