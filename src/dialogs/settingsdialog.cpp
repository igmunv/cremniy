#include "settingsdialog.h"

#include "utils/appsettings.h"
#include "utils/thememanager.h"
#include "utils/themegenerator.h"
#include "customthemeeditordialog.h"

#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProcess>
#include <QPushButton>
#include <QSpinBox>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QInputDialog>

#include "utils/globalwidgetsmanager.h"

static QString resolvedExecutable(const QString &userPath, const QString &exeName)
{
    if (!userPath.trimmed().isEmpty())
        return userPath.trimmed();
    return QStandardPaths::findExecutable(exeName);
}

static bool isRunnableExecutable(const QString &path)
{
    if (path.trimmed().isEmpty())
        return false;
    const QFileInfo fi(path.trimmed());
    return fi.exists() && fi.isFile() && fi.isExecutable();
}

static void setStatusLabel(QLabel *lbl, bool ok, const QString &text)
{
    // Text-only status to avoid adding icon resources.
    // Use a monospace-friendly glyph and color.
    lbl->setText(ok ? QStringLiteral("✓ ") + text : QStringLiteral("✗ ") + text);
    lbl->setStyleSheet(ok ? "color: #39d353;" : "color: #f85149;");
}

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Settings"));
    setModal(true);
    setMinimumSize(760, 520);
    setSizeGripEnabled(true);

    auto *root = new QVBoxLayout(this);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Theme selection
    setupThemeTab();
    
    // Theme row with combo and buttons
    {
        auto *row = new QWidget(this);
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        m_themeCombo = new QComboBox(row);
        m_createThemeBtn = new QPushButton(tr("New…"), row);
        m_createThemeBtn->setFixedWidth(70);
        auto *editThemeBtn = new QPushButton(tr("Edit…"), row);
        editThemeBtn->setFixedWidth(70);
        m_deleteThemeBtn = new QPushButton(tr("Delete…"), row);
        m_deleteThemeBtn->setFixedWidth(80);
        rowLayout->addWidget(m_themeCombo, 1);
        rowLayout->addWidget(m_createThemeBtn);
        rowLayout->addWidget(editThemeBtn);
        rowLayout->addWidget(m_deleteThemeBtn);
        form->addRow(tr("Theme"), row);
        connect(m_themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
                this, &SettingsDialog::onThemeChanged);
        connect(m_createThemeBtn, &QPushButton::clicked, this, &SettingsDialog::onCreateCustomTheme);
        connect(editThemeBtn, &QPushButton::clicked, this, &SettingsDialog::onEditCustomTheme);
        connect(m_deleteThemeBtn, &QPushButton::clicked, this, &SettingsDialog::onDeleteCustomTheme);
    }

    {
        auto *label = new QLabel(tr("Disassembler Settings"), this);
        label->setStyleSheet("font-weight: bold;");
        form->addRow(label);
    }
    m_backendCombo = new QComboBox(this);
    m_backendCombo->addItem(tr("objdump"),  static_cast<int>(AppSettings::DisasmBackend::Objdump));
    m_backendCombo->addItem(tr("radare2"),  static_cast<int>(AppSettings::DisasmBackend::Radare2));
    form->addRow(tr("Disassembler backend"), m_backendCombo);

    // Common disassembler options
    {
        m_insnLimit = new QSpinBox(this);
        m_insnLimit->setRange(50, 200000);
        m_insnLimit->setSingleStep(250);
        m_insnLimit->setToolTip(tr("Maximum number of instructions per section (keeps UI responsive)"));
        form->addRow(tr("Instruction limit/section"), m_insnLimit);

        m_syntaxCombo = new QComboBox(this);
        m_syntaxCombo->addItem(tr("Intel"), static_cast<int>(AppSettings::AsmSyntax::Intel));
        m_syntaxCombo->addItem(tr("AT&T"),  static_cast<int>(AppSettings::AsmSyntax::Att));
        form->addRow(tr("Assembly syntax"), m_syntaxCombo);
    }

    // objdump path row
    {
        auto *row = new QWidget(this);
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        m_objdumpPath = new QLineEdit(row);
        m_objdumpPath->setPlaceholderText(tr("Leave empty to use PATH lookup"));
        m_objdumpStatus = new QLabel(row);
        m_objdumpStatus->setMinimumWidth(150);
        m_objdumpStatus->setTextInteractionFlags(Qt::TextSelectableByMouse);
        auto *browse = new QPushButton(tr("Browse…"), row);
        browse->setFixedWidth(90);
        rowLayout->addWidget(m_objdumpPath, 1);
        rowLayout->addWidget(m_objdumpStatus);
        rowLayout->addWidget(browse);
        form->addRow(tr("objdump path"), row);
        connect(browse, &QPushButton::clicked, this, &SettingsDialog::onBrowseObjdump);
        connect(m_objdumpPath, &QLineEdit::textChanged, this, &SettingsDialog::updateDependencyStatus);
    }

    // radare2 path row
    {
        auto *row = new QWidget(this);
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        m_radare2Path = new QLineEdit(row);
        m_radare2Path->setPlaceholderText(tr("Path to r2 (radare2) executable"));
        m_radare2Status = new QLabel(row);
        m_radare2Status->setMinimumWidth(150);
        m_radare2Status->setTextInteractionFlags(Qt::TextSelectableByMouse);
        auto *browse = new QPushButton(tr("Browse…"), row);
        browse->setFixedWidth(90);
        rowLayout->addWidget(m_radare2Path, 1);
        rowLayout->addWidget(m_radare2Status);
        rowLayout->addWidget(browse);
        form->addRow(tr("radare2 path"), row);
        connect(browse, &QPushButton::clicked, this, &SettingsDialog::onBrowseRadare2);
        connect(m_radare2Path, &QLineEdit::textChanged, this, &SettingsDialog::updateDependencyStatus);
    }

    // 'file' tool is used by objdump backend for arch detection.
    {
        auto *row = new QWidget(this);
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        m_fileStatus = new QLabel(row);
        m_fileStatus->setTextInteractionFlags(Qt::TextSelectableByMouse);
        rowLayout->addWidget(m_fileStatus, 1);
        form->addRow(tr("Dependency: file(1)"), row);
    }

    // radare2 options
    {
        m_r2AnalysisCombo = new QComboBox(this);
        m_r2AnalysisCombo->addItem(tr("None (fast)"), static_cast<int>(AppSettings::Radare2AnalysisLevel::None));
        m_r2AnalysisCombo->addItem(tr("aa (basic)"),  static_cast<int>(AppSettings::Radare2AnalysisLevel::Aa));
        m_r2AnalysisCombo->addItem(tr("aaa (full)"),  static_cast<int>(AppSettings::Radare2AnalysisLevel::Aaa));
        form->addRow(tr("radare2 analysis"), m_r2AnalysisCombo);

        m_r2PreCommands = new QPlainTextEdit(this);
        m_r2PreCommands->setPlaceholderText(tr("Optional r2 commands before JSON queries (one per line). Example:\n"
                                               "e asm.syntax=intel\n"
                                               "e asm.bits=64"));
        m_r2PreCommands->setFixedHeight(90);
        form->addRow(tr("radare2 pre-commands"), m_r2PreCommands);
    }

    root->addLayout(form);

    // buttons
    auto *btnRow = new QHBoxLayout();
    m_testBtn = new QPushButton(tr("Test"), this);
    btnRow->addWidget(m_testBtn);
    m_importBtn = new QPushButton(tr("Import…"), this);
    m_exportBtn = new QPushButton(tr("Export…"), this);
    btnRow->addWidget(m_importBtn);
    btnRow->addWidget(m_exportBtn);
    btnRow->addStretch(1);
    m_okBtn = new QPushButton(tr("OK"), this);
    m_cancelBtn = new QPushButton(tr("Cancel"), this);
    m_okBtn->setDefault(true);
    btnRow->addWidget(m_okBtn);
    btnRow->addWidget(m_cancelBtn);
    root->addLayout(btnRow);

    connect(m_testBtn,   &QPushButton::clicked, this, &SettingsDialog::onTestTools);
    connect(m_exportBtn, &QPushButton::clicked, this, &SettingsDialog::onExportIni);
    connect(m_importBtn, &QPushButton::clicked, this, &SettingsDialog::onImportIni);
    connect(m_okBtn,     &QPushButton::clicked, this, &SettingsDialog::onAccept);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_backendCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::onBackendChanged);
    connect(m_insnLimit, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsDialog::updateDependencyStatus);
    connect(m_syntaxCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsDialog::updateDependencyStatus);
    connect(m_r2AnalysisCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsDialog::updateDependencyStatus);
    connect(m_r2PreCommands, &QPlainTextEdit::textChanged, this, &SettingsDialog::updateDependencyStatus);

    loadFromSettings();
    updateUiEnabledState();
    updateDependencyStatus();
}

void SettingsDialog::onExportIni()
{
    const QString file = QFileDialog::getSaveFileName(
        this,
        tr("Export settings"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/cremniy-settings.ini",
        tr("INI files (*.ini)"));
    if (file.isEmpty()) return;

    QString err;
    if (!AppSettings::exportToIni(file, &err)) {
        QMessageBox::warning(this, tr("Export failed"), err.isEmpty() ? tr("Failed to export settings") : err);
        return;
    }
    QMessageBox::information(this, tr("Export"), tr("Settings exported to:\n%1").arg(file));
}

void SettingsDialog::onImportIni()
{
    const QString file = QFileDialog::getOpenFileName(
        this,
        tr("Import settings"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("INI files (*.ini)"));
    if (file.isEmpty()) return;

    QString err;
    if (!AppSettings::importFromIni(file, &err)) {
        QMessageBox::warning(this, tr("Import failed"), err.isEmpty() ? tr("Failed to import settings") : err);
        return;
    }

    loadFromSettings();
    updateUiEnabledState();
    updateDependencyStatus();
    emit GlobalWidgetsManager::instance().actionTriggered("settingsChanged");
    QMessageBox::information(this, tr("Import"), tr("Settings imported from:\n%1").arg(file));
}

void SettingsDialog::loadFromSettings()
{
    const auto backend = AppSettings::disasmBackend();
    const int want = static_cast<int>(backend);
    int idx = m_backendCombo->findData(want);
    if (idx < 0) idx = 0;
    m_backendCombo->setCurrentIndex(idx);

    m_objdumpPath->setText(AppSettings::objdumpPath());
    m_radare2Path->setText(AppSettings::radare2Path());

    m_insnLimit->setValue(AppSettings::disasmInsnLimitPerSection());

    {
        const int want = static_cast<int>(AppSettings::asmSyntax());
        const int idx = m_syntaxCombo->findData(want);
        m_syntaxCombo->setCurrentIndex(idx < 0 ? 0 : idx);
    }

    {
        const int want = static_cast<int>(AppSettings::radare2AnalysisLevel());
        const int idx = m_r2AnalysisCombo->findData(want);
        m_r2AnalysisCombo->setCurrentIndex(idx < 0 ? 0 : idx);
    }

    m_r2PreCommands->setPlainText(AppSettings::radare2PreCommands().replace(';', '\n'));
    
    // Load theme
    updateThemeList();
    const QString currentTheme = AppSettings::currentTheme();
    const int themeIdx = m_themeCombo->findText(currentTheme);
    if (themeIdx >= 0) {
        m_themeCombo->setCurrentIndex(themeIdx);
    }
}

void SettingsDialog::updateUiEnabledState()
{
    const bool useRadare2 =
        (m_backendCombo->currentData().toInt() == static_cast<int>(AppSettings::DisasmBackend::Radare2));

    // Keep both configurable, but emphasize the active one.
    m_radare2Path->setEnabled(true);
    m_objdumpPath->setEnabled(true);

    m_radare2Path->setToolTip(useRadare2 ? tr("Active backend") : tr("Inactive backend (still configurable)"));
    m_objdumpPath->setToolTip(useRadare2 ? tr("Inactive backend (still configurable)") : tr("Active backend"));

    // r2-specific options enabled only when radare2 is selected
    m_r2AnalysisCombo->setEnabled(useRadare2);
    m_r2PreCommands->setEnabled(useRadare2);
}

void SettingsDialog::onBrowseObjdump()
{
    const QString cur = m_objdumpPath->text().trimmed();
    const QString file = QFileDialog::getOpenFileName(this, tr("Select objdump executable"), cur);
    if (!file.isEmpty())
        m_objdumpPath->setText(file);
}

void SettingsDialog::onBrowseRadare2()
{
    const QString cur = m_radare2Path->text().trimmed();
    const QString file = QFileDialog::getOpenFileName(this, tr("Select radare2 (r2) executable"), cur);
    if (!file.isEmpty())
        m_radare2Path->setText(file);
}

static bool runVersionCheck(const QString &exe, const QStringList &args, QString *out, QString *err)
{
    QProcess p;
    p.start(exe, args);
    if (!p.waitForStarted(2000))
        return false;
    if (!p.waitForFinished(4000))
        return false;
    if (out) *out = QString::fromUtf8(p.readAllStandardOutput()).trimmed();
    if (err) *err = QString::fromUtf8(p.readAllStandardError()).trimmed();
    return p.exitStatus() == QProcess::NormalExit && p.exitCode() == 0;
}

void SettingsDialog::onTestTools()
{
    const QString objdumpExe = resolvedExecutable(m_objdumpPath->text(), "objdump");
    const QString r2Exe      = resolvedExecutable(m_radare2Path->text(), "r2");

    QStringList lines;

    // objdump
    {
        QString out, err;
        const bool ok = !objdumpExe.isEmpty() && runVersionCheck(objdumpExe, {"--version"}, &out, &err);
        lines << (ok ? tr("objdump: OK (%1)").arg(objdumpExe)
                     : tr("objdump: FAIL (%1)").arg(objdumpExe.isEmpty() ? tr("not found") : objdumpExe));
        if (!ok && !err.isEmpty())
            lines << "  " + err;
    }

    // r2
    {
        QString out, err;
        const bool ok = !r2Exe.isEmpty() && runVersionCheck(r2Exe, {"-v"}, &out, &err);
        lines << (ok ? tr("radare2: OK (%1)").arg(r2Exe)
                     : tr("radare2: FAIL (%1)").arg(r2Exe.isEmpty() ? tr("not found") : r2Exe));
        if (ok && !out.isEmpty())
            lines << "  " + out.split('\n').value(0);
        if (!ok && !err.isEmpty())
            lines << "  " + err;
    }

    QMessageBox::information(this, tr("Tool check"), lines.join('\n'));
    updateDependencyStatus();
}

void SettingsDialog::onAccept()
{
    const int backendInt = m_backendCombo->currentData().toInt();
    const auto backend = (backendInt == static_cast<int>(AppSettings::DisasmBackend::Radare2))
        ? AppSettings::DisasmBackend::Radare2
        : AppSettings::DisasmBackend::Objdump;

    AppSettings::setDisasmBackend(backend);
    AppSettings::setObjdumpPath(m_objdumpPath->text());
    AppSettings::setRadare2Path(m_radare2Path->text());

    AppSettings::setDisasmInsnLimitPerSection(m_insnLimit->value());
    AppSettings::setAsmSyntax(static_cast<AppSettings::AsmSyntax>(m_syntaxCombo->currentData().toInt()));
    AppSettings::setRadare2AnalysisLevel(static_cast<AppSettings::Radare2AnalysisLevel>(m_r2AnalysisCombo->currentData().toInt()));

    const QString pre = m_r2PreCommands->toPlainText()
                            .split('\n', Qt::SkipEmptyParts)
                            .join(';');
    AppSettings::setRadare2PreCommands(pre);
    
    // Save and apply theme
    const QString newTheme = m_themeCombo->currentText();
    if (!newTheme.isEmpty()) {
        AppSettings::setCurrentTheme(newTheme);
        ThemeManager::instance().setCurrentTheme(newTheme);
    }

    emit GlobalWidgetsManager::instance().actionTriggered("settingsChanged");
    accept();
}

void SettingsDialog::onBackendChanged(int)
{
    updateUiEnabledState();
    updateDependencyStatus();
}

void SettingsDialog::updateDependencyStatus()
{
    // objdump
    {
        const QString resolved = resolvedExecutable(m_objdumpPath->text(), "objdump");
        const bool ok = isRunnableExecutable(resolved);
        setStatusLabel(m_objdumpStatus, ok, ok ? tr("found") : tr("missing"));
        m_objdumpStatus->setToolTip(ok ? resolved : tr("Not found in PATH and no valid path set"));
    }

    // radare2
    {
        const QString resolved = resolvedExecutable(m_radare2Path->text(), "r2");
        const bool ok = isRunnableExecutable(resolved);
        setStatusLabel(m_radare2Status, ok, ok ? tr("found") : tr("missing"));
        m_radare2Status->setToolTip(ok ? resolved : tr("Not found in PATH and no valid path set"));
    }

    // file(1) dependency
    {
        const QString fileExe = QStandardPaths::findExecutable("file");
        const bool ok = isRunnableExecutable(fileExe);
        setStatusLabel(m_fileStatus, ok, ok ? tr("found") : tr("missing"));
        m_fileStatus->setToolTip(ok ? fileExe : tr("The objdump backend uses 'file -b <path>' for arch detection"));
    }
}

void SettingsDialog::setupThemeTab()
{
    // Placeholder - actual theme UI is created in constructor
}

void SettingsDialog::updateThemeList()
{
    const QString current = m_themeCombo->currentText();
    m_themeCombo->clear();
    
    const QStringList themes = ThemeManager::instance().availableThemes();
    for (const QString &theme : themes) {
        m_themeCombo->addItem(theme);
    }
    
    const int idx = m_themeCombo->findText(current);
    if (idx >= 0) {
        m_themeCombo->setCurrentIndex(idx);
    }
    
    // Disable delete button for built-in themes
    const QString currentTheme = m_themeCombo->currentText();
    m_deleteThemeBtn->setEnabled(!ThemeManager::instance().isBuiltinTheme(currentTheme));
}

void SettingsDialog::onThemeChanged(int)
{
    const QString theme = m_themeCombo->currentText();
    if (!theme.isEmpty()) {
        m_deleteThemeBtn->setEnabled(!ThemeManager::instance().isBuiltinTheme(theme));
    }
}

void SettingsDialog::onCreateCustomTheme()
{
    // Запросить имя темы
    bool ok;
    QString themeName = QInputDialog::getText(
        this,
        tr("Create Custom Theme"),
        tr("Theme name:"),
        QLineEdit::Normal,
        "",
        &ok
    );
    
    if (!ok || themeName.isEmpty()) {
        return;
    }
    
    // Validate theme name
    if (themeName.contains('/') || themeName.contains('\\') || themeName.contains('.')) {
        QMessageBox::warning(this, tr("Invalid Name"), 
            tr("Theme name cannot contain '/', '\\', or '.'"));
        return;
    }
    
    // Проверить, не существует ли уже такая тема
    if (ThemeManager::instance().availableThemes().contains(themeName)) {
        QMessageBox::warning(this, tr("Theme Exists"),
            tr("Theme '%1' already exists").arg(themeName));
        return;
    }
    
    // Создать пустую тему на основе light и открыть редактор
    QString error;
    if (!ThemeManager::instance().createCustomTheme(themeName, "light", &error)) {
        QMessageBox::warning(this, tr("Failed"), error);
        return;
    }
    
    // Открыть редактор для новой темы
    CustomThemeEditorDialog editor(themeName, this);
    if (editor.exec() == QDialog::Accepted) {
        const ThemeColors colors = editor.colors();
        const QString qss = ThemeGenerator::generateQSS(colors);
        
        if (ThemeManager::instance().saveCustomTheme(themeName, qss, &error)) {
            updateThemeList();
            m_themeCombo->setCurrentText(themeName);
            ThemeManager::instance().loadTheme(themeName);
            QMessageBox::information(this, tr("Success"), 
                tr("Custom theme '%1' created successfully").arg(themeName));
        } else {
            QMessageBox::warning(this, tr("Failed"), error);
        }
    } else {
        // Если пользователь отменил редактор, удалить созданную тему
        ThemeManager::instance().deleteCustomTheme(themeName);
    }
}

void SettingsDialog::onEditCustomTheme()
{
    const QString theme = m_themeCombo->currentText();
    
    if (theme.isEmpty()) {
        QMessageBox::information(this, tr("Select Theme"),
            tr("Please select a theme to edit"));
        return;
    }
    
    CustomThemeEditorDialog editor(theme, this);
    if (editor.exec() == QDialog::Accepted) {
        const ThemeColors colors = editor.colors();
        const QString qss = ThemeGenerator::generateQSS(colors);
        
        QString error;
        if (ThemeManager::instance().saveCustomTheme(theme, qss, &error)) {
            ThemeManager::instance().loadTheme(theme);
            QMessageBox::information(this, tr("Success"),
                tr("Theme '%1' updated successfully").arg(theme));
        } else {
            QMessageBox::warning(this, tr("Failed"), error);
        }
    }
}

void SettingsDialog::onDeleteCustomTheme()
{
    const QString theme = m_themeCombo->currentText();
    
    if (ThemeManager::instance().isBuiltinTheme(theme)) {
        QMessageBox::warning(this, tr("Error"), 
            tr("Cannot delete built-in theme '%1'").arg(theme));
        return;
    }
    
    const int result = QMessageBox::question(this, tr("Delete Theme"),
        tr("Are you sure you want to delete the custom theme '%1'?").arg(theme),
        QMessageBox::Yes | QMessageBox::No);
    
    if (result == QMessageBox::Yes) {
        QString error;
        if (ThemeManager::instance().deleteCustomTheme(theme, &error)) {
            updateThemeList();
            QMessageBox::information(this, tr("Success"),
                tr("Custom theme '%1' deleted successfully").arg(theme));
        } else {
            QMessageBox::warning(this, tr("Failed"), error);
        }
    }
}

