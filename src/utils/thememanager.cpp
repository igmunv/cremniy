#include "thememanager.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QApplication>
#include <QDebug>

ThemeManager::ThemeManager()
{
    initializeThemes();
}

ThemeManager& ThemeManager::instance()
{
    static ThemeManager mgr;
    return mgr;
}

void ThemeManager::initializeThemes()
{
    m_builtinThemes = {"light", "dark"};
    ensureThemesDirectory();
    
    // Try to load the last used theme or default to 'dark'
    QString lastTheme = m_builtinThemes.contains("dark") ? "dark" : m_builtinThemes.first();
    loadTheme(lastTheme);
}

QString ThemeManager::getThemesDirectory() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/themes";
}

void ThemeManager::ensureThemesDirectory() const
{
    QDir dir(getThemesDirectory());
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

QStringList ThemeManager::availableThemes() const
{
    QStringList themes = m_builtinThemes;
    
    QDir customDir(getThemesDirectory());
    if (customDir.exists()) {
        const QStringList filters = {"*.qss"};
        const QFileInfoList files = customDir.entryInfoList(filters, QDir::Files);
        for (const QFileInfo &fi : files) {
            const QString name = fi.baseName();
            if (!name.startsWith('.')) {  // Skip hidden files
                themes.append(name);
            }
        }
    }
    
    return themes;
}

ThemeManager::Theme ThemeManager::currentTheme() const
{
    return m_currentTheme;
}

bool ThemeManager::loadTheme(const QString &themeName, QString *error)
{
    if (themeName.isEmpty()) {
        if (error) *error = QApplication::translate("ThemeManager", "Theme name is empty");
        return false;
    }
    
    QString styleSheet = getThemeStyleSheet(themeName, error);
    if (styleSheet.isEmpty() && !m_currentTheme.name.isEmpty()) {
        return false;  // Keep current theme if load fails
    }
    
    m_currentTheme.name = themeName;
    m_currentTheme.path = getThemePath(themeName);
    m_currentTheme.isBuiltin = isBuiltinTheme(themeName);
    
    if (!styleSheet.isEmpty()) {
        qobject_cast<QApplication*>(QApplication::instance())->setStyleSheet(styleSheet);
    }
    
    return true;
}

bool ThemeManager::setCurrentTheme(const QString &themeName)
{
    QString error;
    return loadTheme(themeName, &error);
}

QString ThemeManager::getThemeStyleSheet(const QString &themeName, QString *error) const
{
    QString path = getThemePath(themeName);
    
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        if (error) *error = QCoreApplication::translate("ThemeManager", "Cannot open theme file: %1").arg(path);
        return QString();
    }
    
    QString content = QString::fromUtf8(file.readAll());
    file.close();
    
    return content;
}

QString ThemeManager::getThemePath(const QString &themeName) const
{
    if (m_builtinThemes.contains(themeName)) {
        return QString(":/styles/%1.qss").arg(themeName);
    }
    
    return QString("%1/%2.qss").arg(getThemesDirectory(), themeName);
}

bool ThemeManager::isBuiltinTheme(const QString &themeName) const
{
    return m_builtinThemes.contains(themeName);
}

bool ThemeManager::createCustomTheme(const QString &themeName, const QString &baseTheme, QString *error)
{
    if (themeName.isEmpty() || themeName.contains('/') || themeName.contains('\\')) {
        if (error) *error = QCoreApplication::translate("ThemeManager", "Invalid theme name");
        return false;
    }
    
    if (availableThemes().contains(themeName)) {
        if (error) *error = QCoreApplication::translate("ThemeManager", "Theme already exists");
        return false;
    }
    
    QString baseStyleSheet = getThemeStyleSheet(baseTheme, error);
    if (baseStyleSheet.isEmpty()) {
        return false;
    }
    
    return saveCustomTheme(themeName, baseStyleSheet, error);
}

bool ThemeManager::saveCustomTheme(const QString &themeName, const QString &styleSheet, QString *error)
{
    if (isBuiltinTheme(themeName)) {
        if (error) *error = QCoreApplication::translate("ThemeManager", "Cannot overwrite built-in theme");
        return false;
    }
    
    ensureThemesDirectory();
    
    QString path = QString("%1/%2.qss").arg(getThemesDirectory(), themeName);
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        if (error) *error = QCoreApplication::translate("ThemeManager", "Cannot write theme file: %1").arg(path);
        return false;
    }
    
    file.write(styleSheet.toUtf8());
    file.close();
    
    return true;
}

bool ThemeManager::deleteCustomTheme(const QString &themeName, QString *error)
{
    if (isBuiltinTheme(themeName)) {
        if (error) *error = QCoreApplication::translate("ThemeManager", "Cannot delete built-in theme");
        return false;
    }
    
    QString path = QString("%1/%2.qss").arg(getThemesDirectory(), themeName);
    QFile file(path);
    if (!file.exists()) {
        if (error) *error = QCoreApplication::translate("ThemeManager", "Theme file not found: %1").arg(path);
        return false;
    }
    
    if (!file.remove()) {
        if (error) *error = QCoreApplication::translate("ThemeManager", "Cannot delete theme file: %1").arg(path);
        return false;
    }
    
    // If deleted theme was active, switch to default
    if (m_currentTheme.name == themeName) {
        loadTheme("dark");
    }
    
    return true;
}
