#ifndef CUSTOMTHEMEEDITORDIALOG_H
#define CUSTOMTHEMEEDITORDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>
#include <QScrollArea>
#include <QLineEdit>
#include "themecolors.h"

class ColorPickerWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ColorPickerWidget(const QString &name, const QColor &initialColor, QWidget *parent = nullptr);
    
    QColor color() const;
    void setColor(const QColor &c);
    
signals:
    void colorChanged(const QColor &color);
    
private slots:
    void onColorButtonClicked();
    
private:
    void updateStyleSheet();
    
private:
    QLabel *m_nameLabel;
    QPushButton *m_colorButton;
    QColor m_color;
};

class CustomThemeEditorDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CustomThemeEditorDialog(const QString &themeName, QWidget *parent = nullptr);
    
    QString themeName() const { return m_themeName; }
    ThemeColors colors() const;
    
private slots:
    void onColorChanged();
    void onPresetLight();
    void onPresetDark();
    void onReset();
    void onAccept();
    
private:
    void setupUI();
    void createColorPicker(const QString &name, QColor ThemeColors::*member);
    void updatePreview();
    
    QString m_themeName;
    ThemeColors m_colors;
    QMap<QString, ColorPickerWidget*> m_colorPickers;
    QLabel *m_previewLabel;
    QWidget *m_previewWidget;
};

#endif // CUSTOMTHEMEEDITORDIALOG_H
