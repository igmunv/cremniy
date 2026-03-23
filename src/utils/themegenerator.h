#ifndef THEMEGENERATOR_H
#define THEMEGENERATOR_H

#include <QString>
#include "themecolors.h"

class ThemeGenerator
{
public:
    static QString generateQSS(const ThemeColors &colors);
    
private:
    static QString colorToQss(const QColor &color);
};

#endif // THEMEGENERATOR_H
