#include "gui.h"
#include "configwindow.h"
#include <QApplication>
#include <QClipboard>
#include <QHotkey>
#include <cstddef>

void setRegistered(std::map<std::string, QHotkey *> hotkeys, bool registered) {
    for (auto const &x : hotkeys) {
        (x.second)->setRegistered(registered);
    }
}

void removeWhitespaces(char *text)
{
    int amount = 0;
    for (size_t i = 0; text[i]; i++)
        if (text[i] != ' ') {
            text[amount++] = text[i];
        }
    text[amount] = 0;
}

QHotkey *setupOCRHotkey(QString sequence, char *callback(ORIENTATION orn),
                        ORIENTATION orn) {
    QHotkey *hotkey = new QHotkey(QKeySequence(sequence), true, qApp);
    QObject::connect(hotkey, &QHotkey::activated, qApp, [=]() {
        char *text = callback(orn);
        removeWhitespaces(text);
        qApp->clipboard()->setText(text);
    });
    return hotkey;
}

void startGui(QApplication *app, char *interactive(ORIENTATION orn),
              char *prevOcr(ORIENTATION orn)) {
    QSettings settings("gazou", "gazou");

    QString verticalHotkey =
        settings.value("Hotkeys/verticalOCR", "Alt+A").toString();
    QString horizontalHotkey =
        settings.value("Hotkeys/horizontalOCR", "Alt+D").toString();
    QString prevOCRHotkey =
        settings.value("Hotkeys/repeatOCR", "Alt+S").toString();

    QHotkey *vKey = setupOCRHotkey(verticalHotkey, interactive, VERTICAL);
    QHotkey *hKey = setupOCRHotkey(horizontalHotkey, interactive, HORIZONTAL);
    QHotkey *prevKey = setupOCRHotkey(prevOCRHotkey, prevOcr);

    std::map<std::string, QHotkey *> hotkeys = {
        {"verticalOCR", vKey}, {"horizontalOCR", hKey}, {"repeatOCR", prevKey}};

    new ConfigWindow(hotkeys);

    app->setQuitOnLastWindowClosed(false);
    setRegistered(hotkeys, true);
}
