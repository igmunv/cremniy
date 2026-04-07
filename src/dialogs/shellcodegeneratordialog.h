#ifndef SHELLCODEGENERATORDIALOG_H
#define SHELLCODEGENERATORDIALOG_H

#include <QDialog>

class QComboBox;

class ShellcodeGeneratorDialog : public QDialog {
  Q_OBJECT
public:
  explicit ShellcodeGeneratorDialog(QWidget *parent = nullptr);

private:
  QComboBox *m_langStyle = nullptr;
};

#endif // SHELLCODEGENERATORDIALOG_H
