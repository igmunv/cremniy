#include "customthemeeditordialog.h"
#include "utils/themegenerator.h"
#include <QFormLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QApplication>

// ColorPickerWidget

ColorPickerWidget::ColorPickerWidget(const QString &name, const QColor &initialColor, QWidget *parent)
    : QWidget(parent), m_color(initialColor)
{
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    m_nameLabel = new QLabel(name);
    m_nameLabel->setMinimumWidth(150);
    
    m_colorButton = new QPushButton();
    m_colorButton->setFixedSize(60, 40);
    updateStyleSheet();
    
    layout->addWidget(m_nameLabel);
    layout->addStretch();
    layout->addWidget(m_colorButton);
    
    connect(m_colorButton, &QPushButton::clicked, this, &ColorPickerWidget::onColorButtonClicked);
}

QColor ColorPickerWidget::color() const
{
    return m_color;
}

void ColorPickerWidget::setColor(const QColor &c)
{
    if (m_color != c) {
        m_color = c;
        updateStyleSheet();
        emit colorChanged(c);
    }
}

void ColorPickerWidget::updateStyleSheet()
{
    m_colorButton->setStyleSheet(
        QString("QPushButton { background-color: %1; border: 2px solid #ccc; }").arg(m_color.name())
    );
}

void ColorPickerWidget::onColorButtonClicked()
{
    QColor newColor = QColorDialog::getColor(m_color, this, tr("Select Color"));
    if (newColor.isValid()) {
        setColor(newColor);
    }
}

// CustomThemeEditorDialog

CustomThemeEditorDialog::CustomThemeEditorDialog(const QString &themeName, QWidget *parent)
    : QDialog(parent), m_themeName(themeName), m_colors(ThemeColors::light())
{
    setWindowTitle(tr("Edit Theme: %1").arg(themeName));
    setModal(true);
    setMinimumSize(900, 700);
    
    setupUI();
}

void CustomThemeEditorDialog::setupUI()
{
    auto *root = new QVBoxLayout(this);
    
    // Название темы
    {
        auto *row = new QHBoxLayout();
        row->addWidget(new QLabel(tr("Theme Name:")));
        auto *nameEdit = new QLineEdit();
        nameEdit->setText(m_themeName);
        nameEdit->setReadOnly(true);
        row->addWidget(nameEdit);
        root->addLayout(row);
    }
    
    // Кнопки предустановок
    {
        auto *row = new QHBoxLayout();
        auto *lightBtn = new QPushButton(tr("Light Preset"));
        auto *darkBtn = new QPushButton(tr("Dark Preset"));
        auto *resetBtn = new QPushButton(tr("Reset"));
        row->addWidget(lightBtn);
        row->addWidget(darkBtn);
        row->addWidget(resetBtn);
        row->addStretch();
        
        connect(lightBtn, &QPushButton::clicked, this, &CustomThemeEditorDialog::onPresetLight);
        connect(darkBtn, &QPushButton::clicked, this, &CustomThemeEditorDialog::onPresetDark);
        connect(resetBtn, &QPushButton::clicked, this, &CustomThemeEditorDialog::onReset);
        
        root->addLayout(row);
    }
    
    // Прокручиваемая область с выбором цветов
    {
        auto *scroll = new QScrollArea();
        scroll->setWidgetResizable(true);
        
        auto *colorWidget = new QWidget();
        auto *colorLayout = new QVBoxLayout(colorWidget);
        
        // Основные цвета
        {
            auto *group = new QGroupBox(tr("Main Colors"));
            auto *layout = new QVBoxLayout(group);
            
            auto *bgPicker = new ColorPickerWidget(tr("Background"), m_colors.backgroundColor);
            auto *textPicker = new ColorPickerWidget(tr("Text"), m_colors.textColor);
            auto *accentPicker = new ColorPickerWidget(tr("Accent"), m_colors.accentColor);
            
            m_colorPickers["backgroundColor"] = bgPicker;
            m_colorPickers["textColor"] = textPicker;
            m_colorPickers["accentColor"] = accentPicker;
            
            layout->addWidget(bgPicker);
            layout->addWidget(textPicker);
            layout->addWidget(accentPicker);
            
            colorLayout->addWidget(group);
            
            connect(bgPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
            connect(textPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
            connect(accentPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
        }
        
        // Кнопки
        {
            auto *group = new QGroupBox(tr("Buttons"));
            auto *layout = new QVBoxLayout(group);
            
            auto *bgPicker = new ColorPickerWidget(tr("Background"), m_colors.buttonBackground);
            auto *hoverPicker = new ColorPickerWidget(tr("Hover"), m_colors.buttonHoverBackground);
            auto *pressPicker = new ColorPickerWidget(tr("Pressed"), m_colors.buttonPressedBackground);
            auto *borderPicker = new ColorPickerWidget(tr("Border"), m_colors.buttonBorder);
            auto *textPicker = new ColorPickerWidget(tr("Text"), m_colors.buttonTextColor);
            
            m_colorPickers["buttonBackground"] = bgPicker;
            m_colorPickers["buttonHoverBackground"] = hoverPicker;
            m_colorPickers["buttonPressedBackground"] = pressPicker;
            m_colorPickers["buttonBorder"] = borderPicker;
            m_colorPickers["buttonTextColor"] = textPicker;
            
            layout->addWidget(bgPicker);
            layout->addWidget(hoverPicker);
            layout->addWidget(pressPicker);
            layout->addWidget(borderPicker);
            layout->addWidget(textPicker);
            
            colorLayout->addWidget(group);
            
            connect(bgPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
            connect(hoverPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
            connect(pressPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
            connect(borderPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
            connect(textPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
        }
        
        // Поля ввода
        {
            auto *group = new QGroupBox(tr("Input Fields"));
            auto *layout = new QVBoxLayout(group);
            
            auto *bgPicker = new ColorPickerWidget(tr("Background"), m_colors.inputBackground);
            auto *borderPicker = new ColorPickerWidget(tr("Border"), m_colors.inputBorder);
            auto *focusPicker = new ColorPickerWidget(tr("Focus Border"), m_colors.inputFocusBorder);
            auto *textPicker = new ColorPickerWidget(tr("Text"), m_colors.inputTextColor);
            auto *selPicker = new ColorPickerWidget(tr("Selection"), m_colors.inputSelectionBackground);
            
            m_colorPickers["inputBackground"] = bgPicker;
            m_colorPickers["inputBorder"] = borderPicker;
            m_colorPickers["inputFocusBorder"] = focusPicker;
            m_colorPickers["inputTextColor"] = textPicker;
            m_colorPickers["inputSelectionBackground"] = selPicker;
            
            layout->addWidget(bgPicker);
            layout->addWidget(borderPicker);
            layout->addWidget(focusPicker);
            layout->addWidget(textPicker);
            layout->addWidget(selPicker);
            
            colorLayout->addWidget(group);
            
            connect(bgPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
            connect(borderPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
            connect(focusPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
            connect(textPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
            connect(selPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
        }
        
        // Списки
        {
            auto *group = new QGroupBox(tr("Lists"));
            auto *layout = new QVBoxLayout(group);
            
            auto *bgPicker = new ColorPickerWidget(tr("Background"), m_colors.listBackground);
            auto *altPicker = new ColorPickerWidget(tr("Alternate"), m_colors.listAlternateBackground);
            auto *selectedPicker = new ColorPickerWidget(tr("Selected"), m_colors.listSelectedBackground);
            auto *hoverPicker = new ColorPickerWidget(tr("Hover"), m_colors.listHoverBackground);
            auto *textPicker = new ColorPickerWidget(tr("Text"), m_colors.listTextColor);
            
            m_colorPickers["listBackground"] = bgPicker;
            m_colorPickers["listAlternateBackground"] = altPicker;
            m_colorPickers["listSelectedBackground"] = selectedPicker;
            m_colorPickers["listHoverBackground"] = hoverPicker;
            m_colorPickers["listTextColor"] = textPicker;
            
            layout->addWidget(bgPicker);
            layout->addWidget(altPicker);
            layout->addWidget(selectedPicker);
            layout->addWidget(hoverPicker);
            layout->addWidget(textPicker);
            
            colorLayout->addWidget(group);
            
            connect(bgPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
            connect(altPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
            connect(selectedPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
            connect(hoverPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
            connect(textPicker, &ColorPickerWidget::colorChanged, this, &CustomThemeEditorDialog::onColorChanged);
        }
        
        colorLayout->addStretch();
        scroll->setWidget(colorWidget);
        root->addWidget(scroll, 1);
    }
    
    // Кнопки OK/Cancel
    {
        auto *row = new QHBoxLayout();
        auto *okBtn = new QPushButton(tr("Save"));
        auto *cancelBtn = new QPushButton(tr("Cancel"));
        
        row->addStretch();
        row->addWidget(okBtn);
        row->addWidget(cancelBtn);
        
        connect(okBtn, &QPushButton::clicked, this, &CustomThemeEditorDialog::onAccept);
        connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
        
        root->addLayout(row);
    }
}

void CustomThemeEditorDialog::onColorChanged()
{
    // Обновить структуру цветов
    for (auto it = m_colorPickers.begin(); it != m_colorPickers.end(); ++it) {
        const QString &key = it.key();
        const QColor &color = it.value()->color();
        
        if (key == "backgroundColor") m_colors.backgroundColor = color;
        else if (key == "textColor") m_colors.textColor = color;
        else if (key == "buttonBackground") m_colors.buttonBackground = color;
        else if (key == "buttonHoverBackground") m_colors.buttonHoverBackground = color;
        else if (key == "buttonPressedBackground") m_colors.buttonPressedBackground = color;
        else if (key == "buttonBorder") m_colors.buttonBorder = color;
        else if (key == "buttonTextColor") m_colors.buttonTextColor = color;
        else if (key == "inputBackground") m_colors.inputBackground = color;
        else if (key == "inputBorder") m_colors.inputBorder = color;
        else if (key == "inputFocusBorder") m_colors.inputFocusBorder = color;
        else if (key == "inputTextColor") m_colors.inputTextColor = color;
        else if (key == "inputSelectionBackground") m_colors.inputSelectionBackground = color;
        else if (key == "listBackground") m_colors.listBackground = color;
        else if (key == "listAlternateBackground") m_colors.listAlternateBackground = color;
        else if (key == "listSelectedBackground") m_colors.listSelectedBackground = color;
        else if (key == "listHoverBackground") m_colors.listHoverBackground = color;
        else if (key == "listTextColor") m_colors.listTextColor = color;
        else if (key == "accentColor") m_colors.accentColor = color;
    }
}

void CustomThemeEditorDialog::onPresetLight()
{
    m_colors = ThemeColors::light();
    
    // Обновить все picker'ы
    for (auto it = m_colorPickers.begin(); it != m_colorPickers.end(); ++it) {
        const QString &key = it.key();
        ColorPickerWidget *picker = it.value();
        
        if (key == "backgroundColor") picker->setColor(m_colors.backgroundColor);
        else if (key == "textColor") picker->setColor(m_colors.textColor);
        else if (key == "buttonBackground") picker->setColor(m_colors.buttonBackground);
        else if (key == "buttonHoverBackground") picker->setColor(m_colors.buttonHoverBackground);
        else if (key == "buttonPressedBackground") picker->setColor(m_colors.buttonPressedBackground);
        else if (key == "buttonBorder") picker->setColor(m_colors.buttonBorder);
        else if (key == "buttonTextColor") picker->setColor(m_colors.buttonTextColor);
        else if (key == "inputBackground") picker->setColor(m_colors.inputBackground);
        else if (key == "inputBorder") picker->setColor(m_colors.inputBorder);
        else if (key == "inputFocusBorder") picker->setColor(m_colors.inputFocusBorder);
        else if (key == "inputTextColor") picker->setColor(m_colors.inputTextColor);
        else if (key == "inputSelectionBackground") picker->setColor(m_colors.inputSelectionBackground);
        else if (key == "listBackground") picker->setColor(m_colors.listBackground);
        else if (key == "listAlternateBackground") picker->setColor(m_colors.listAlternateBackground);
        else if (key == "listSelectedBackground") picker->setColor(m_colors.listSelectedBackground);
        else if (key == "listHoverBackground") picker->setColor(m_colors.listHoverBackground);
        else if (key == "listTextColor") picker->setColor(m_colors.listTextColor);
        else if (key == "accentColor") picker->setColor(m_colors.accentColor);
    }
}

void CustomThemeEditorDialog::onPresetDark()
{
    m_colors = ThemeColors::dark();
    
    for (auto it = m_colorPickers.begin(); it != m_colorPickers.end(); ++it) {
        const QString &key = it.key();
        ColorPickerWidget *picker = it.value();
        
        if (key == "backgroundColor") picker->setColor(m_colors.backgroundColor);
        else if (key == "textColor") picker->setColor(m_colors.textColor);
        else if (key == "buttonBackground") picker->setColor(m_colors.buttonBackground);
        else if (key == "buttonHoverBackground") picker->setColor(m_colors.buttonHoverBackground);
        else if (key == "buttonPressedBackground") picker->setColor(m_colors.buttonPressedBackground);
        else if (key == "buttonBorder") picker->setColor(m_colors.buttonBorder);
        else if (key == "buttonTextColor") picker->setColor(m_colors.buttonTextColor);
        else if (key == "inputBackground") picker->setColor(m_colors.inputBackground);
        else if (key == "inputBorder") picker->setColor(m_colors.inputBorder);
        else if (key == "inputFocusBorder") picker->setColor(m_colors.inputFocusBorder);
        else if (key == "inputTextColor") picker->setColor(m_colors.inputTextColor);
        else if (key == "inputSelectionBackground") picker->setColor(m_colors.inputSelectionBackground);
        else if (key == "listBackground") picker->setColor(m_colors.listBackground);
        else if (key == "listAlternateBackground") picker->setColor(m_colors.listAlternateBackground);
        else if (key == "listSelectedBackground") picker->setColor(m_colors.listSelectedBackground);
        else if (key == "listHoverBackground") picker->setColor(m_colors.listHoverBackground);
        else if (key == "listTextColor") picker->setColor(m_colors.listTextColor);
        else if (key == "accentColor") picker->setColor(m_colors.accentColor);
    }
}

void CustomThemeEditorDialog::onReset()
{
    m_colors = ThemeColors::light();
    onPresetLight();
}

void CustomThemeEditorDialog::onAccept()
{
    accept();
}

ThemeColors CustomThemeEditorDialog::colors() const
{
    return m_colors;
}
