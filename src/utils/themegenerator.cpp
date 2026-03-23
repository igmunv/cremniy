#include "themegenerator.h"

QString ThemeGenerator::colorToQss(const QColor &color)
{
    return color.name();
}

QString ThemeGenerator::generateQSS(const ThemeColors &colors)
{
    QString qss;
    
    qss += QString(
        "/* Auto-generated theme */\n"
        "QWidget {\n"
        "    background-color: %1;\n"
        "    color: %2;\n"
        "    font-family: \"JetBrains Mono\", \"Consolas\", monospace;\n"
        "    font-size: 12px;\n"
        "}\n\n"
    ).arg(colorToQss(colors.backgroundColor), colorToQss(colors.textColor));
    
    qss += QString(
        "QHexView {\n"
        "    background-color: %1;\n"
        "    font-size: 16px;\n"
        "    border: none;\n"
        "}\n\n"
    ).arg(colorToQss(colors.backgroundColor));
    
    // Кнопки
    qss += QString(
        "QPushButton {\n"
        "    background-color: %1;\n"
        "    border: 1px solid %2;\n"
        "    padding: 6px 12px;\n"
        "    color: %3;\n"
        "}\n\n"
        "QPushButton:hover {\n"
        "    background-color: %4;\n"
        "    border: 1px solid %5;\n"
        "}\n\n"
        "QPushButton:pressed {\n"
        "    background-color: %6;\n"
        "    border: 2px solid %5;\n"
        "    font-weight: bold;\n"
        "}\n\n"
    ).arg(colorToQss(colors.buttonBackground),
          colorToQss(colors.buttonBorder),
          colorToQss(colors.buttonTextColor),
          colorToQss(colors.buttonHoverBackground),
          colorToQss(colors.accentColor),
          colorToQss(colors.buttonPressedBackground));
    
    // ComboBox
    qss += QString(
        "QComboBox {\n"
        "    background-color: %1;\n"
        "    border: 1px solid %2;\n"
        "    padding: 2px;\n"
        "    color: %3;\n"
        "}\n\n"
        "QComboBox:hover {\n"
        "    background-color: %4;\n"
        "    border: 1px solid %5;\n"
        "}\n\n"
        "QComboBox::drop-down {\n"
        "    border: 0px;\n"
        "    width: 0px;\n"
        "}\n\n"
        "QComboBox::down-arrow {\n"
        "    image: none;\n"
        "}\n\n"
    ).arg(colorToQss(colors.buttonBackground),
          colorToQss(colors.buttonBorder),
          colorToQss(colors.buttonTextColor),
          colorToQss(colors.buttonHoverBackground),
          colorToQss(colors.accentColor));
    
    // Поля ввода
    qss += QString(
        "QLineEdit, QTextEdit, QPlainTextEdit {\n"
        "    background-color: %1;\n"
        "    border: 1px solid %2;\n"
        "    selection-background-color: %3;\n"
        "    padding: 2px;\n"
        "    color: %4;\n"
        "}\n\n"
        "QPlainTextEdit {\n"
        "    background-color: %1;\n"
        "}\n\n"
        "QLineEdit:hover {\n"
        "    background-color: %5;\n"
        "    border: 1px solid %6;\n"
        "}\n\n"
    ).arg(colorToQss(colors.inputBackground),
          colorToQss(colors.inputBorder),
          colorToQss(colors.inputSelectionBackground),
          colorToQss(colors.inputTextColor),
          colorToQss(colors.backgroundColor),
          colorToQss(colors.inputFocusBorder));
    
    // Списки
    qss += QString(
        "QListView, QTreeView, QTableView {\n"
        "    background-color: %1;\n"
        "    border: 1px solid %2;\n"
        "    alternate-background-color: %3;\n"
        "    color: %4;\n"
        "}\n\n"
    ).arg(colorToQss(colors.listBackground),
          colorToQss(colors.borderColor),
          colorToQss(colors.listAlternateBackground),
          colorToQss(colors.listTextColor));
    
    // Tabs
    qss += QString(
        "QTabWidget::pane {\n"
        "    border: 1px solid %1;\n"
        "}\n\n"
    ).arg(colorToQss(colors.borderColor));
    
    // TreeView
    qss += QString(
        "QTreeView::item:selected {\n"
        "    background-color: %1;\n"
        "    color: %2;\n"
        "}\n\n"
        "QTreeView::item:hover:!selected {\n"
        "    background-color: %3;\n"
        "}\n\n"
    ).arg(colorToQss(colors.listSelectedBackground),
          colorToQss(colors.listTextColor),
          colorToQss(colors.listHoverBackground));
    
    // Меню
    qss += QString(
        "QMenuBar::item {\n"
        "    background: transparent;\n"
        "    color: %1;\n"
        "}\n\n"
        "QMenuBar::item:selected {\n"
        "    background-color: %2;\n"
        "}\n\n"
        "QMenuBar::item:hover {\n"
        "    background-color: %3;\n"
        "}\n\n"
        "QMenu {\n"
        "    background-color: %4;\n"
        "    color: %1;\n"
        "    border: 1px solid %5;\n"
        "}\n\n"
        "QMenu::item:selected {\n"
        "    background: %6;\n"
        "}\n\n"
    ).arg(colorToQss(colors.menuTextColor),
          colorToQss(colors.menuSelectedBackground),
          colorToQss(colors.menuHoverBackground),
          colorToQss(colors.menuBackground),
          colorToQss(colors.menuBorder),
          colorToQss(colors.menuHoverBackground));
    
    // Прокрутка
    qss += QString(
        "QScrollBar:vertical {\n"
        "    background: %1;\n"
        "    width: 16px;\n"
        "}\n\n"
        "QScrollBar::handle:vertical {\n"
        "    background: %2;\n"
        "    border: 1px solid %3;\n"
        "    border-radius: 2px;\n"
        "}\n\n"
        "QScrollBar::handle:vertical:hover {\n"
        "    background: %4;\n"
        "}\n\n"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
        "    background: none;\n"
        "    height: 16px;\n"
        "}\n\n"
        "QScrollBar:horizontal {\n"
        "    background: %1;\n"
        "    height: 16px;\n"
        "}\n\n"
        "QScrollBar::handle:horizontal {\n"
        "    background: %2;\n"
        "    border: 1px solid %3;\n"
        "    border-radius: 2px;\n"
        "}\n\n"
        "QScrollBar::handle:horizontal:hover {\n"
        "    background: %4;\n"
        "}\n\n"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {\n"
        "    background: none;\n"
        "    width: 16px;\n"
        "}\n"
    ).arg(colorToQss(colors.backgroundColor),
          colorToQss(colors.buttonBackground),
          colorToQss(colors.borderColor),
          colorToQss(colors.buttonHoverBackground));
    
    return qss;
}
