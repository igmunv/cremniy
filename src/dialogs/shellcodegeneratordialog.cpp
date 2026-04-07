#include "shellcodegeneratordialog.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSize>
#include <QVBoxLayout>
#include <cstdint>

struct ShellcodeStyle {
  const char *label;
  int8_t id;
};

static const std::vector<ShellcodeStyle> shellcodeStyles = {{"C", 0},
                                                            {"C++", 1}};

ShellcodeGeneratorDialog::ShellcodeGeneratorDialog(QWidget *parent)
    : QDialog(parent) {
  setWindowTitle(tr("Shellcode Generator"));
  setModal(false);
  setMinimumSize(QSize(1400, 760));

  auto *root = new QVBoxLayout(this);
  auto *topRow = new QHBoxLayout();

  topRow->addWidget(new QLabel(tr("Format:"), this));
  m_langStyle = new QComboBox(this);
  for (const auto &style : shellcodeStyles) {
    m_langStyle->addItem(style.label, style.id);
  }
  m_langStyle->setMinimumWidth(50);

  topRow->addWidget(m_langStyle);
  topRow->addStretch(1);

  root->addLayout(topRow);
  root->addStretch(1);
}
