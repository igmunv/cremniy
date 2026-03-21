#ifndef TOOLTAB_H
#define TOOLTAB_H

#include "filecontext.h"
#include <qstringview.h>
#include <qwidget.h>

class ToolTab : public QWidget {
    Q_OBJECT

protected:

    /**
     * @brief Хеш изначальных данных
     *
     * Используется для отслеживания изменения изначальных данных
    */
    uint m_dataHash = 0;

    /**
     * @brief Контекст текущего файла
    */
    FileContext* m_fileContext;

public:
    explicit ToolTab(QWidget* parent = nullptr) : QWidget(parent) {}

    /**
     * @brief Получить название инструмента для вкладки
    */
    virtual QString toolName() const = 0;

    /**
     * @brief Получить лого инструмента для вкладки
    */
    virtual QIcon toolIcon() const = 0;

public slots:

    /**
     * @brief Установить файл инструменту
    */
    virtual void setFile(QString filepath) = 0;

    /**
     * @brief Установить данные из файла во вкладку
    */
    virtual void setTabData() = 0;

    /**
     * @brief Сохраняет данные из вкладки в файл
    */
    virtual void saveTabData() = 0;

signals:

    /**
     * @brief Обновить данные из файла во всех вкладках
     *
     * Эмиттируется, когда нужно обновить данные на всех вкладках интерфейса
     * Например, при сохранении данных текущего ToolTab в файл, необходимо обновить данные на всех остальных ToolTab
    */
    void refreshDataAllTabsSignal();

    /**
     * @brief Изменение изначальных данных
     *
     * Эмиттируется, каждый рах когда происходит изменение данных и они не равны изначальным данным
    */
    void modifyData(bool modified);

    /**
     * @brief Изначальные данные не отличаются от текущих
     *
     * Эмиттируется, каждый рах когда происходит изменение данных и они равны изначальным данным
     * Также эмиттируется после вызова функции setTabData, чтобы убрать звезду
    */
    void dataEqual();

};

#endif // TOOLTAB_H
