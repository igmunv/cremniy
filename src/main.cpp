#include <QApplication>
#include <QTranslator>
#include <QCoreApplication>
#include <QFile>
#include <QIcon>

#include "app/WelcomeWindow/welcomeform.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("cremniy");

    QTranslator translator;
    if (translator.load(QLocale("ru"),"app", "_",QCoreApplication::applicationDirPath() + "/../Resources/translations/"))
        QApplication::installTranslator(&translator);

    QCoreApplication::setApplicationName("Cremniy");
    a.setWindowIcon(QIcon(":/icons/icon.png"));

    // Themes
    QIcon::setThemeSearchPaths({":/icons"});
    QIcon::setThemeName("phoicons");         // маленькими буквами!

    QFile file(":/styles/style.qss");
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open the style file: " << file.errorString();
        return 1;
    }
    QString styleSheet = QLatin1String(file.readAll());
    a.setStyleSheet(styleSheet);

    WelcomeForm wf;
    wf.show();
    return QCoreApplication::exec();
}