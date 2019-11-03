#ifndef VKEYBOARD_H
#define VKEYBOARD_H

#include <QWidget>
#include <QKeyEvent>

namespace Ui {
class VKeyboard;
}

class VKeyboard : public QWidget
{
    Q_OBJECT

public:
    explicit VKeyboard(QWidget *parent = 0);
    ~VKeyboard();

    void jump_rule(int th, int keyType);

    bool eventFilter (QObject *obj, QEvent *event);

private:
    Ui::VKeyboard *ui;
};

#endif // VKEYBOARD_H
