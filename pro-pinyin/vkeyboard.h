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
    explicit VKeyboard(QWidget *parent = 0);
    ~VKeyboard();

    void jump_rule(int th, int keyType);
    bool clicked_rule(QPushButton *pb);

    bool eventFilter (QObject *obj, QEvent *event);

private:
    Ui::VKeyboard *ui;
    MyTextEdit *mte = NULL;
};


#endif // VKEYBOARD_H
