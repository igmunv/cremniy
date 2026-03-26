#ifndef DISASSEMBLERWORKER_H
#define DISASSEMBLERWORKER_H

#include <QObject>
#include <QString>
#include <QVector>

struct DisasmInstruction {
    QString address;
    QString bytes;
    QString mnemonic;
    QString operands;
};

struct DisasmSection {
    QString name;
    QVector<DisasmInstruction> instructions;
};

struct DisasmFunction {
    QString name;
    QString address; // string like 0x...
};

struct DisasmString {
    QString address; // string like 0x...
    QString value;   // decoded string
};

class DisassemblerWorker : public QObject
{
    Q_OBJECT

public:
    explicit DisassemblerWorker(QObject *parent = nullptr);

public slots:
    void disassemble(const QString &filePath, const QString &arch);
    void cancel();

signals:
    void sectionFound(const DisasmSection &section);
    void functionsFound(const QVector<DisasmFunction> &funcs);
    void stringsFound(const QVector<DisasmString> &strings);
    void finished();
    void errorOccurred(const QString &errorMsg);
    void progressUpdated(int percent);
    void logLine(const QString &line);   // diagnostic log line

private:
    bool m_cancelled = false;
    friend class Radare2Backend;

    QVector<DisasmSection> parseSections(const QByteArray &output);
    static QString detectArch(const QString &filePath);
};

#endif // DISASSEMBLERWORKER_H
