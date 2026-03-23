#include <QApplication>
#include <QCoreApplication>

#include "app/WelcomeWindow/welcomeform.h"
#include "utils/thememanager.h"
#include "utils/appsettings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("cremniy");
    QCoreApplication::setApplicationName("Cremniy");
    a.setWindowIcon(QIcon(":/icons/icon.png"));

    // Load theme based on settings, default to dark
    QString theme = AppSettings::currentTheme();
    if (theme.isEmpty()) {
        theme = "dark";
    }
    ThemeManager::instance().loadTheme(theme);

    WelcomeForm wf;
    wf.show();
    return QCoreApplication::exec();
}
