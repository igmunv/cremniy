#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QString>
#include <QStringList>

class ThemeManager
{
public:
    struct Theme {
        QString name;
        QString path;
        bool isBuiltin;
    };

    static ThemeManager& instance();

    QStringList availableThemes() const;
    Theme currentTheme() const;
    bool setCurrentTheme(const QString &themeName);
    bool loadTheme(const QString &themeName, QString *error = nullptr);
    
    // Custom theme management
    bool createCustomTheme(const QString &themeName, const QString &baseTheme = "light", QString *error = nullptr);
    bool deleteCustomTheme(const QString &themeName, QString *error = nullptr);
    bool saveCustomTheme(const QString &themeName, const QString &styleSheet, QString *error = nullptr);
    
    QString getThemeStyleSheet(const QString &themeName, QString *error = nullptr) const;
    QString getThemePath(const QString &themeName) const;
    bool isBuiltinTheme(const QString &themeName) const;

private:
    explicit ThemeManager();

    void initializeThemes();
    QString getThemesDirectory() const;
    void ensureThemesDirectory() const;

    Theme m_currentTheme;
    QStringList m_builtinThemes;
};

#endif // THEMEMANAGER_H
