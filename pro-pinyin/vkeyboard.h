#ifndef VKEYBOARD_H
#define VKEYBOARD_H

#include <QWidget>
#include <QKeyEvent>
#include <QPushButton>

namespace Ui {
class VKeyboard;
}

#include <QTextEdit>
class MyTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    void startCursor(void);
};

class VKeyboard : public QWidget
{
    Q_OBJECT

public:

    enum KB_TYPE{
        ANY = 0,
        //以下项可叠加使用
        NUMBER = 0x01,
        LOWER = 0x02,
        CAPITAL = 0x04,
        PINTING = 0x08,
        SYMBOL = 0x10,
        //以下项不可叠加使用
        IP = 0x21,
        URL = 0x22,
        USERNAME = 0x23,
        PASSWORD = 0x24,
        FILENAME = 0x25,
    };

    explicit VKeyboard(int type = ANY, bool space = false, bool multiLine = false, QWidget *parent = 0);
    ~VKeyboard();

    void grid_rule(QObject *obj, int keyType);
    bool clicked_rule(QPushButton *pb);
    bool eventFilter (QObject *obj, QEvent *event);

protected:
    void keyboard_grid_init();

private:
    Ui::VKeyboard *ui;
    MyTextEdit *textEdit = NULL;
    const QString number = "1234567890";
    const QString lower = "qwertyuiopasdfghjklzxcvbnm";
    const QString capital = "QWERTYUIOPASDFGHJKLZXCVBNM";
    const QString symbol = "";
};


#endif // VKEYBOARD_H
