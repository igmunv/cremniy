#ifndef RAWPAGE_H
#define RAWPAGE_H

#include "QHexView/qhexview.h"
#include "formatpage.h"

class RAWPage : public FormatPage
{ Q_OBJECT

private:
    QHexView* m_hexViewWidget;

public:
    explicit RAWPage(QWidget *parent = nullptr);

    QString pageName() const override { return "RAW"; }

    void setPageData(QByteArray& data) override;
    QByteArray getPageData() const override;

};

#endif // RAWPAGE_H
