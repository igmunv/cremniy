#ifndef TERMINALWIDGET_H
#define TERMINALWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <KodoTerm/KodoTerm.hpp>

class TerminalWidget : public QWidget {
    Q_OBJECT
public:
    explicit TerminalWidget(QWidget *parent = nullptr);
    void applyTheme(bool isDark);
    explicit TerminalWidget(QWidget *parent = nullptr, const QString &workingDirectory = QString());
    ~TerminalWidget();

private slots:
    void onReadyRead();
    void onProcessError(QProcess::ProcessError error);

private:
    void startShell(); // Метод для отложенного запуска
    
    KodoTerm *m_terminal;
    bool m_isStarted = false; 

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    QPlainTextEdit *m_display;
    QProcess *m_process;
    QString m_workingDirectory;
    
    int m_lastPromptPos = 0;
    
    QStringList m_history;
    int m_historyIndex = 0;
};

#endif