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
        KB_USER = 0x20,//该模式时启用userCandidate
    };

    //value: 传入被编辑的字符串
    //type: 编辑类型,由KB_TYPE组合而成
    //userCandidate: 用户的自定义的输入内容,每项用空格分开(例如"A B C"),type包含KB_USER时启用
    //space/multiLine/tab: 是否允许空格,回车,Tab输入
    //示例:
    //    IP: VInput(&ret, VInput::KB_NUMBER|VInput::KB_USER, ".");
    //    email: VInput(&ret, VInput::KB_LOWER|VInput::KB_NUMBER|VInput::KB_USER, ". @");//注意候选项要用空格分开
    //    任意输入: VInput(&ret, VInput::KB_ANY);
    //    自定义一些特殊符号: VInput(&ret, VInput::KB_USER, "\u2714 \u2718 \u266C");
    VInput(QString *value = NULL, int type = KB_ANY, QString userCandidate = "", bool space = false, bool multiLine = false, bool tab = false, QWidget *parent = 0);
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
    void comm_pushButton_clicked(bool c);
    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_pushButton_tips_up_clicked();
    
    void on_pushButton_tips_down_clicked();

    void on_pushButton_tips_left_clicked();

    void on_pushButton_tips_right_clicked();
    
    void on_pushButton_tips_movL_clicked();
    
    void on_pushButton_tips_movR_clicked();
    
    void on_pushButton_tips_ent_clicked();
    
    void on_pushButton_tips_del_clicked();
    
    void on_pushButton_tips_select_clicked();

private:
    Ui::VInput *ui;
    MyTextEdit textEdit;//光标常闪输入框
    bool useSpace;//记录传参,是否启用空格
    bool useMultiLine;//记录传参,是否启用回车
    bool useTab;
    int kb_type = 0;//记录传参,输入类型
    QString *returnString;//返回写入
    QString pinyinCandidate = "";//缓存拼音输入
    bool pinyin_run = false;//拼音输入法运行状态

    const QString kb_number = "1 2 3 4 5 6 7 8 9 0";
    const QString kb_lower = "q w e r t y u i o p a s d f g h j k l z x c v b n m";
    const QString kb_capital = "Q W E R T Y U I O P A S D F G H J K L Z X C V B N M";
    const QString kb_symbol = 
        //--- 1页 ---
        "+ - * / = : \" ; ' , ( ) [ ] { } < > ~ ! ? . _ @ % "
        //--- 2页 ---
        "$ # && ^ \\ | ` \u00B0 \u2103 \u2109 "
        "\u0024 \u00A2 \u00A3 \u00A4 \u20AC \u00A5 \u20B1 \u20B9 \u2260 "
        "\u00AB \u00BB \u03A9 \u03A8 \u03A6 \u03A3 "
        //--- 3页 ---
        "\u03A0 \u039B \u0398 \u0394 \u2190 \u2192 \u2191 \u2193 \u2194 \u2195 "
        "\u21C4 \u21C5 \u25B2 \u25BC \u25C4 \u25BA \u2764 \u2605 \u2606";
    int kb_symbol_count = 0;
    QString kb_user = "";
    int kb_user_count = 0;
};


#endif // VInput_H
