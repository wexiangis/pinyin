#ifndef VINPUT_H
#define VINPUT_H

#include <QDialog>
#include <QKeyEvent>
#include <QPushButton>
#include <QListWidgetItem>

namespace Ui {
class VInput;
}

#include <QTextEdit>
class MyTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    void startCursor(void);
};

class VInput : public QDialog
{
    Q_OBJECT

public:

    enum KB_TYPE{
        KB_ANY = 0,
        //以下项可叠加使用
        KB_NUMBER = 0x01,
        KB_LOWER = 0x02,
        KB_CAPITAL = 0x04,
        KB_PINYIN = 0x08,
        KB_SYMBOL = 0x10,
        KB_USER = 0x20,
    };

    VInput(QString *value = NULL, int type = KB_ANY, QString userCandidate = "", bool space = false, bool multiLine = false, QWidget *parent = 0);
    ~VInput();

protected:
    void grid_link();
    void grid_load(KB_TYPE type);
    void grid_jump(QObject *obj, int keyType);
    bool clicked_rule(QPushButton *pb);

    void pinyin_input(QString str);
    bool pinyin_delete(void);
    void pinyin_clean(void);
    void pinyin_open(void);
    void pinyin_close(void);
    bool pinyin_isOpen(void);
    bool pinyin_move(bool isRight);
    void pinyin_refreshList(void);

    bool eventFilter (QObject *obj, QEvent *event);

protected slots:
    void on_pushButton_clicked(bool c);
    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::VInput *ui;
    MyTextEdit textEdit;//光标常闪输入框
    bool useSpace;//记录传参,是否启用空格
    bool useMultiLine;//记录传参,是否启用回车
    int kb_type;//记录传参,输入类型
    QString *returnString;//返回写入
    QString pinyinCandidate = "";//缓存拼音输入
    bool pinyin_run = false;//拼音输入法运行状态

    const QString kb_number = "1234567890";
    const QString kb_lower = "qwertyuiopasdfghjklzxcvbnm";
    const QString kb_capital = "QWERTYUIOPASDFGHJKLZXCVBNM";
    const QString kb_symbol = "+-*/=:\";',()[]{}<>~!?._@#%\\|";
    QString kb_user;
};


#endif // VInput_H
