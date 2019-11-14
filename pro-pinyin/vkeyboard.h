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
        PINYIN = 0x08,
        SYMBOL = 0x10,
    };

    VKeyboard(QString *value = NULL, int type = ANY, bool space = false, bool multiLine = false, QWidget *parent = 0);
    ~VKeyboard();

protected:
    void grid_link();
    void grid_load(KB_TYPE type);
    void grid_jump(QObject *obj, int keyType);
    bool clicked_rule(QPushButton *pb);

    bool eventFilter (QObject *obj, QEvent *event);

private slots:
    void on_pushButton_clicked(bool c);

private:
    Ui::VKeyboard *ui;
    MyTextEdit *textEdit;
    bool useSpace;
    bool useMultiLine;
    int kb_type;
    QString *targetString;
    const QString kb_number = "1234567890";
    const QString kb_lower = "qwertyuiopasdfghjklzxcvbnm";
    const QString kb_capital = "QWERTYUIOPASDFGHJKLZXCVBNM";
    const QString kb_symbol = "+-*/=:\";',()[]{}<>~!?._@#%\\|";
};


#endif // VKEYBOARD_H
