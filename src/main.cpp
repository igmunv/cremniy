#include <QApplication>
#include <QCoreApplication>

#include "app/WelcomeWindow/welcomeform.h"
#include "utils/themeservice.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("cremniy");
    QCoreApplication::setApplicationName("Cremniy");
    a.setWindowIcon(QIcon(":/icons/icon.png"));

    ThemeService::applyFromSettings();

    WelcomeForm wf;
    wf.show();
    return QCoreApplication::exec();
}
