#pragma once

#include <functional>
#include <QMap>
#include <QString>

class ToolTab;

class ToolTabFactory {

public:
    using Creator = std::function<ToolTab*()>;

    static ToolTabFactory& instance();

    void registerTab(const QString& id, Creator creator);
    ToolTab* create(const QString& id);
    QStringList availableTabs() const;

private:
    QMap<QString, Creator> m_creators;

};
