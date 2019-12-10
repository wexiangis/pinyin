#include "vinput.h"
#include "ui_vinput.h"

#include "pinyinime.h"
#include <QDebug>

#define VINPUT_MODE 0

#if(VINPUT_MODE)
#include "main_argv.h"
extern QString argv_dictPath;
extern QString argv_dictUserPath;
#else
QString argv_dictPath = "/usr/local/fw6118/dict/dict_pinyin.dat";
QString argv_dictUserPath = "/usr/local/fw6118/dict/dict_pinyin_user.dat";
#endif

//---------- 放在pushButton的UserData中的跳转指针 ----------

//键盘上下左右跳转指针,放置到pushButton的userData中
class KeyboardGrid : public QObjectUserData
{
public:
    KeyboardGrid(QPushButton *u, QPushButton *d, QPushButton *l, QPushButton *r)
    {
        udlr[0] = l;
        udlr[1] = u;
        udlr[2] = r;
        udlr[3] = d;
    };
    QPushButton *udlr[4];
};

//---------- 光标一直闪烁的输入框 ----------

void MyTextEdit::startCursor(void)
{
    QFocusEvent fe(QEvent::FocusIn, Qt::TabFocusReason);
    focusInEvent (&fe);
}

//---------- 拼音库测试 ----------

using namespace ime_pinyin;

void google_pinyin_test()
{
    //---------- 拼音接口 ---------
    if(!im_open_decoder(argv_dictPath.toLocal8Bit ().data (), argv_dictUserPath.toLocal8Bit ().data ()))
//    if(!im_open_decoder((const char *)"qrc:/dict/dict_pinyin.dat", (const char *)"qrc:/dict/dict_pinyin_user.dat"))
        qDebug() << "im_open_decoder: err!";
    //设置输入输出上限
    //max_sps_len: 输入拼音字母的最大长度
    //max_hzs_len: 解码中文字符的最大长度
    im_set_max_lens(16, 16);
    //清除缓冲
    im_flush_cache();
    //重置
    im_reset_search();
    //
    int search_res =  im_search((const char*)"zhonghaida", 10);
    qDebug() << "search_res: " << search_res;

    //对当前查找结果执行删除操作， 然后再重新查找
    //pos: 拼音字母的位置或者是搜索结果ID
    //is_pos_in_splid: 申明pos参数是字母位置还是搜索结果ID
    //clear_fixed_this_step:
    //返回候选数量
    //size_t im_delsearch(size_t pos, bool is_pos_in_splid, bool clear_fixed_this_step);

    for(int i = 0; i < search_res; i++)
    {
        char16 cand_str[64] = {0};
        if(im_get_candidate(i, cand_str, 64))
            qDebug() << "find: " << QString::fromUtf16 (cand_str);
    }
    im_close_decoder();
}

//---------- 虚拟键盘 ----------

void VInput::pinyin_refreshList(void)
{
    ui->listWidget->clear ();
    if(pinyinCandidate.size () > 0)
    {
        int search_res =  im_search(pinyinCandidate.toLocal8Bit ().data (), pinyinCandidate.size ());
        for(int i = 0; i < search_res; i++)
        {
            char16 cand_str[64] = {0};
            if(im_get_candidate(i, cand_str, 64))
                ui->listWidget->addItem (QString::fromUtf16 (cand_str));
        }
        //
        if(search_res < 1)
            ui->listWidget->addItem (pinyinCandidate);
    }
    if(ui->listWidget->count () > 0)
        ui->listWidget->setCurrentRow (0);
    //
    if(pinyinCandidate.size () > 0)
    {
        ui->listWidget->show ();
        ui->label_pinyin->show ();
    }
    else
    {
        ui->listWidget->hide ();
        ui->label_pinyin->hide ();
    }
}

void VInput::pinyin_open(void)
{
    if(pinyin_run)
    {
        im_close_decoder();
        pinyin_run = false;
    }
    if(!im_open_decoder(argv_dictPath.toLocal8Bit ().data (), argv_dictUserPath.toLocal8Bit ().data ()))
    {
        qDebug() << "im_open_decoder: err!";
        pinyin_close ();
    }
    else
    {
        qDebug() << "im_open_decoder: success";
        //设置输入输出上限
        //max_sps_len: 输入拼音字母的最大长度
        //max_hzs_len: 解码中文字符的最大长度
        im_set_max_lens(16, 16);
        //清除缓冲
        im_flush_cache();
        //重置
        im_reset_search();

        pinyin_run = true;
        ui->listWidget->show ();
        ui->label_pinyin->show ();
    }
}

void VInput::pinyin_close(void)
{
    if(pinyin_run)
        im_close_decoder();
    ui->listWidget->hide ();
    ui->label_pinyin->hide ();
    pinyin_run = false;
}

void VInput::pinyin_input(QString str)
{
    pinyinCandidate.append (str);
    ui->label_pinyin->setText (pinyinCandidate);
    pinyin_refreshList();
}

bool VInput::pinyin_delete(void)
{
    if(pinyinCandidate.size () > 0)
    {
        pinyinCandidate.remove (pinyinCandidate.size ()-1, 1);
        ui->label_pinyin->setText (pinyinCandidate);
        pinyin_refreshList();
        return true;
    }
    return false;
}

void VInput::pinyin_clean(void)
{
    ui->listWidget->clear ();
    pinyinCandidate = "";
    ui->label_pinyin->setText (pinyinCandidate);
    ui->listWidget->hide ();
    ui->label_pinyin->hide ();
}

bool VInput::pinyin_isOpen(void)
{
    return pinyin_run;
}

bool VInput::pinyin_move(bool isRight)
{
    if(!ui->listWidget->isHidden () && ui->listWidget->count () > 0)
    {
        if(isRight)
        {
            int c = ui->listWidget->currentRow () + 1;
            if(c >= ui->listWidget->count ())
                c = 0;
            ui->listWidget->setCurrentRow (c);
        }
        else
        {
            if(!ui->listWidget->isHidden () && ui->listWidget->count () > 0)
            {
                int c = ui->listWidget->currentRow () - 1;
                if(c < 0)
                    c = ui->listWidget->count () - 1;
                ui->listWidget->setCurrentRow (c);
            }
        }
        return true;
    }
    return false;
}

VInput::VInput(QString *value, int type, QString userCandidate, bool space, bool multiLine, bool tab, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    returnString(value),
    kb_type(type),
    useSpace(space),
    useMultiLine(multiLine),
    useTab(tab),
    kb_user(userCandidate),
    ui(new Ui::VInput)
{
    // this->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    this->setGeometry (0, 0, this->width (), this->height ());
    //输入字符范围限制
    pinyin_close ();
    if(type != KB_ANY)
    {
        //底部栏按键使能选择
        if(!(type&KB_LOWER) && !(type&KB_CAPITAL))
            ui->pushButton_30->setEnabled (false);
        if(!(type&KB_NUMBER))
            ui->pushButton_29->setEnabled (false);
        if(!(type&KB_PINYIN))
            ui->pushButton_31->setEnabled (false);
        if(!(type&KB_SYMBOL) && !(type&KB_USER) && !useTab)
            ui->pushButton_32->setEnabled (false);
        //默认键盘选择
        if(!(type&KB_LOWER) && (type&KB_CAPITAL))
        {
            grid_load(KB_CAPITAL);
            ui->pushButton_30->setText ("-ABC-");
        }
        else if((type&KB_LOWER) && !(type&KB_CAPITAL))
        {
            grid_load(KB_LOWER);
            ui->pushButton_30->setText ("-abc-");
        }
        else if((type&KB_LOWER) && (type&KB_CAPITAL))
        {
            grid_load(KB_LOWER);
            ui->pushButton_30->setText ("ABC");
        }
        else if((type&KB_NUMBER))
        {
            grid_load(KB_NUMBER);
        }
        else if((type&KB_PINYIN))
        {
            grid_load(KB_PINYIN);
            pinyin_clean();
            pinyin_open ();
        }
        else if((type&KB_USER))
        {
            grid_load(KB_USER);
        }
        else if((type&KB_SYMBOL))
        {
            grid_load(KB_SYMBOL);
        }
    }
    else
    {
        useSpace = true;
        useMultiLine = true;
        useTab = true;
    }
    //空格键
    ui->pushButton_20->setEnabled (useSpace);
    //回车键
    ui->pushButton_28->setEnabled (useMultiLine);
    //网格跳转属性初始化
    grid_link();
    //光标闪烁的输入行
    if(returnString)
        textEdit.setPlainText (*returnString);
    textEdit.setFocusPolicy (Qt::NoFocus);
    textEdit.setLineWrapMode (QTextEdit::NoWrap);
    textEdit.setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    textEdit.setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    ui->verticalLayout->insertWidget (0, &textEdit);
    textEdit.startCursor();
    textEdit.moveCursor (QTextCursor::End);
    //遍历控件,连接到eventFilter()
    //鼠标回调
    foreach (QObject *obj, ui->gridLayoutWidget->children())
    {
        if(obj->objectName ().indexOf("pushButton") == 0)
        {
//            qDebug() << obj->objectName ();
            obj->installEventFilter (this);
            QPushButton *pb = (QPushButton*)obj;
            connect(pb, SIGNAL(clicked(bool)), this, SLOT(comm_pushButton_clicked(bool)));
        }
    }
    //
    ui->pushButton_34->setFocus ();
}

VInput::~VInput()
{
    delete ui;
    ui = NULL;
}

//把每个按键的上下左右链接起来
void VInput::grid_link()
{
    ui->pushButton_1->setUserData (0, new KeyboardGrid(ui->pushButton_28, ui->pushButton_11, ui->pushButton_10, ui->pushButton_2));
    ui->pushButton_2->setUserData (0, new KeyboardGrid(ui->pushButton_29, ui->pushButton_12, ui->pushButton_1, ui->pushButton_3));
    ui->pushButton_3->setUserData (0, new KeyboardGrid(ui->pushButton_30, ui->pushButton_13, ui->pushButton_2, ui->pushButton_4));
    ui->pushButton_4->setUserData (0, new KeyboardGrid(ui->pushButton_31, ui->pushButton_14, ui->pushButton_3, ui->pushButton_5));
    ui->pushButton_5->setUserData (0, new KeyboardGrid(ui->pushButton_32, ui->pushButton_15, ui->pushButton_4, ui->pushButton_6));
    ui->pushButton_6->setUserData (0, new KeyboardGrid(ui->pushButton_33, ui->pushButton_16, ui->pushButton_5, ui->pushButton_7));
    ui->pushButton_7->setUserData (0, new KeyboardGrid(ui->pushButton_34, ui->pushButton_17, ui->pushButton_6, ui->pushButton_8));
    ui->pushButton_8->setUserData (0, new KeyboardGrid(ui->pushButton_34, ui->pushButton_18, ui->pushButton_7, ui->pushButton_9));
    ui->pushButton_9->setUserData (0, new KeyboardGrid(ui->pushButton_34, ui->pushButton_19, ui->pushButton_8, ui->pushButton_10));
    ui->pushButton_10->setUserData (0, new KeyboardGrid(ui->pushButton_34, ui->pushButton_20, ui->pushButton_9, ui->pushButton_1));

    ui->pushButton_11->setUserData (0, new KeyboardGrid(ui->pushButton_1, ui->pushButton_21, ui->pushButton_20, ui->pushButton_12));
    ui->pushButton_12->setUserData (0, new KeyboardGrid(ui->pushButton_2, ui->pushButton_22, ui->pushButton_11, ui->pushButton_13));
    ui->pushButton_13->setUserData (0, new KeyboardGrid(ui->pushButton_3, ui->pushButton_23, ui->pushButton_12, ui->pushButton_14));
    ui->pushButton_14->setUserData (0, new KeyboardGrid(ui->pushButton_4, ui->pushButton_24, ui->pushButton_13, ui->pushButton_15));
    ui->pushButton_15->setUserData (0, new KeyboardGrid(ui->pushButton_5, ui->pushButton_25, ui->pushButton_14, ui->pushButton_16));
    ui->pushButton_16->setUserData (0, new KeyboardGrid(ui->pushButton_6, ui->pushButton_26, ui->pushButton_15, ui->pushButton_17));
    ui->pushButton_17->setUserData (0, new KeyboardGrid(ui->pushButton_7, ui->pushButton_27, ui->pushButton_16, ui->pushButton_18));
    ui->pushButton_18->setUserData (0, new KeyboardGrid(ui->pushButton_8, ui->pushButton_27, ui->pushButton_17, ui->pushButton_19));
    ui->pushButton_19->setUserData (0, new KeyboardGrid(ui->pushButton_9, ui->pushButton_27, ui->pushButton_18, ui->pushButton_20));
    ui->pushButton_20->setUserData (0, new KeyboardGrid(ui->pushButton_10, ui->pushButton_27, ui->pushButton_19, ui->pushButton_11));

    ui->pushButton_21->setUserData (0, new KeyboardGrid(ui->pushButton_11, ui->pushButton_28, ui->pushButton_27, ui->pushButton_22));
    ui->pushButton_22->setUserData (0, new KeyboardGrid(ui->pushButton_12, ui->pushButton_29, ui->pushButton_21, ui->pushButton_23));
    ui->pushButton_23->setUserData (0, new KeyboardGrid(ui->pushButton_13, ui->pushButton_30, ui->pushButton_22, ui->pushButton_24));
    ui->pushButton_24->setUserData (0, new KeyboardGrid(ui->pushButton_14, ui->pushButton_31, ui->pushButton_23, ui->pushButton_25));
    ui->pushButton_25->setUserData (0, new KeyboardGrid(ui->pushButton_15, ui->pushButton_32, ui->pushButton_24, ui->pushButton_26));
    ui->pushButton_26->setUserData (0, new KeyboardGrid(ui->pushButton_16, ui->pushButton_33, ui->pushButton_25, ui->pushButton_27));
    ui->pushButton_27->setUserData (0, new KeyboardGrid(ui->pushButton_17, ui->pushButton_34, ui->pushButton_26, ui->pushButton_21));

    ui->pushButton_28->setUserData (0, new KeyboardGrid(ui->pushButton_21, ui->pushButton_1, ui->pushButton_34, ui->pushButton_29));
    ui->pushButton_29->setUserData (0, new KeyboardGrid(ui->pushButton_22, ui->pushButton_2, ui->pushButton_28, ui->pushButton_30));
    ui->pushButton_30->setUserData (0, new KeyboardGrid(ui->pushButton_23, ui->pushButton_3, ui->pushButton_29, ui->pushButton_31));
    ui->pushButton_31->setUserData (0, new KeyboardGrid(ui->pushButton_24, ui->pushButton_4, ui->pushButton_30, ui->pushButton_32));
    ui->pushButton_32->setUserData (0, new KeyboardGrid(ui->pushButton_25, ui->pushButton_5, ui->pushButton_31, ui->pushButton_33));
    ui->pushButton_33->setUserData (0, new KeyboardGrid(ui->pushButton_26, ui->pushButton_6, ui->pushButton_32, ui->pushButton_34));
    ui->pushButton_34->setUserData (0, new KeyboardGrid(ui->pushButton_27, ui->pushButton_7, ui->pushButton_33, ui->pushButton_28));
}

//按输入类型加载键盘
void VInput::grid_load(KB_TYPE type)
{
    QStringList tarString;
    int tarCount = 0, count = 0, countMax = 25;//每页最多26个字符
    //
    switch(type)
    {
        case KB_NUMBER:
            tarString = kb_number.split (" ");
            break;
        case KB_PINYIN:
        case KB_LOWER:
            tarString = kb_lower.split (" ");
            break;
        case KB_CAPITAL:
            tarString = kb_capital.split (" ");
            break;
        case KB_SYMBOL:
            tarString = kb_symbol.split (" ");
            tarCount =  kb_symbol_count;
            //
            kb_symbol_count += countMax;
            if(kb_symbol_count >= tarString.size ())
                kb_symbol_count = 0;
            break;
        case KB_USER:
            tarString = kb_user.split (" ");
            tarCount =  kb_user_count;
            //
            kb_user_count += countMax;
            if(kb_user_count >= tarString.size ())
                kb_user_count = 0;
            break;
        default:
            return;
    }
    //对于有多页的界面,当加载了别的窗口后,
    //当前窗口的候选字符串数组置0,这样下次打开时才会从第一页打开
    if(type != KB_SYMBOL && type != KB_USER)
    {
        kb_symbol_count = 0;
        kb_user_count = 0;
        ui->pushButton_32->setText ("其它");
    }
    else if(tarString.size () > countMax)
    {
        ui->pushButton_32->setText (
                QString::number (tarCount/countMax+1)+
                "/"+
                QString::number (tarString.size ()/countMax+1)+
                "页");
    }
    //
    QPushButton *pb;
    foreach (QObject *obj, ui->gridLayoutWidget->children())
    {
        if(obj->objectName ().indexOf("pushButton") == 0)
        {
            pb = (QPushButton*)obj;
            if(tarCount < tarString.size ())
            {
                pb->setText (tarString[tarCount]);
                pb->setEnabled (true);
            }
            else
            {
                pb->setText (" ");
                pb->setEnabled (false);
            }
            //
            tarCount ++;
            count++;
            //
            if(count > countMax)
                break;
        }
    }
    //
    if(type == KB_SYMBOL || type == KB_USER)
    {
        ui->pushButton_27->setText ("Tab");
        ui->pushButton_27->setEnabled (useTab);
    }
    ui->pushButton_20->setText ("Space");
    ui->pushButton_20->setEnabled (useSpace);
    ui->pushButton_28->setText ("Enter");
    ui->pushButton_28->setEnabled (useMultiLine);
}

//键盘上的焦点的跳转规则
void VInput::grid_jump(QObject *obj, int keyType)
{
    KeyboardGrid *kg = (KeyboardGrid*)(obj->userData (0));
    if(kg && keyType >= Qt::Key_Left && keyType <= Qt::Key_Down)
    {
        int order = keyType - Qt::Key_Left;
        QPushButton *pb = kg->udlr[order];
        int count = 0;//防止无限循环
        while(kg && pb && count < 34)
        {
            if(pb->isEnabled ())
            {
                pb->setFocus ();
                break;
            }
            kg = (KeyboardGrid*)(pb->userData (0));
            if(kg)
                pb = kg->udlr[order];
            count ++;
        }
        //
        if(pb == obj)
        {
            if(keyType == Qt::Key_Up || keyType == Qt::Key_Left)
                focusNextPrevChild (false);
            else
                focusNextPrevChild (true);
        }
    }
}

//鼠标点击键盘事件也转到clicked_rule()
void VInput::comm_pushButton_clicked(bool c)
{
    clicked_rule((QPushButton*)QApplication::focusWidget());
}

//每个按键的点击事件都归到这里统一管理
bool VInput::clicked_rule(QPushButton *pb)
{
    //空格
    if(pb == ui->pushButton_20 && ui->pushButton_20->text() == "Space")
    {
        textEdit.insertPlainText (" ");
    }
    //换行
    else if(pb == ui->pushButton_28 && ui->pushButton_28->text() == "Enter")
    {
        textEdit.insertPlainText ("\n");
    }
    //Tab
    else if(pb == ui->pushButton_27 && ui->pushButton_27->text() == "Tab")
    {
        textEdit.insertPlainText ("\t");
    }
    //数字
    else if(pb == ui->pushButton_29)
    {
        grid_load(KB_NUMBER);
        pinyin_close ();
    }
    //字母
    else if(pb == ui->pushButton_30)
    {
        if(ui->pushButton_30->text () == "abc")
        {
            grid_load(KB_LOWER);
            ui->pushButton_30->setText ("ABC");
        }
        else if(ui->pushButton_30->text () == "ABC")
        {
            grid_load(KB_CAPITAL);
            ui->pushButton_30->setText ("abc");
        }
        else
        {
            if((kb_type&KB_CAPITAL))
            {
                grid_load(KB_CAPITAL);
                ui->pushButton_30->setText ("-ABC-");
            }
            else
            {
                grid_load(KB_LOWER);
                ui->pushButton_30->setText ("-abc-");
            }
        }
        pinyin_close ();
    }
    //拼音
    else if(pb == ui->pushButton_31)
    {
        grid_load(KB_LOWER);
        pinyin_clean();
        pinyin_open ();
    }
    //符号
    else if(pb == ui->pushButton_32)
    {
        if(kb_type&KB_USER)
            grid_load(KB_USER);
        else
            grid_load(KB_SYMBOL);
        pinyin_close ();
    }
    //保存结束
    else if(pb == ui->pushButton_33)
    {
        if(returnString)
        {
            *returnString = textEdit.toPlainText ();
            qDebug() << *returnString;
        }
        this->done (1);
    }
    //取消返回
    else if(pb == ui->pushButton_34)
    {
        this->done (0);
    }
    //键值输入
    else if(pb)
    {
        if(pinyin_isOpen ())
            pinyin_input (pb->text ());
        else
        {
            QString val = pb->text ();
            //&符号特殊处理
            if(val.size () > 1 && val == "&&")
                    val = "&";
            //
            textEdit.insertPlainText (val);
        }
    }
    else
        return false;
    //保持输入框光标
    textEdit.ensureCursorVisible();
    return true;
}

bool VInput::eventFilter (QObject *obj, QEvent *event)
{
#if(VINPUT_MODE)
    if(argv_screenOff_count < 0){
        argv_screenOff_count = 0;
        return true;
    }argv_screenOff_count = 0;
#endif

    if(this->isHidden ())
        return true;

    if(obj->parent() == ui->gridLayoutWidget)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            switch(keyEvent->key())
            {
                case Qt::Key_Up:
                case Qt::Key_Down:
                case Qt::Key_Left:
                case Qt::Key_Right:
                    grid_jump(obj, keyEvent->key());
                    break;
                case Qt::Key_Equal://左移
                    if(!pinyin_move(false))
                        textEdit.moveCursor (QTextCursor::Left);
                    break;
                case Qt::Key_Minus://右移
                    if(!pinyin_move(true))
                        textEdit.moveCursor (QTextCursor::Right);
                    break;
                case Qt::Key_Z://删除
                    if(!pinyin_delete ())
                        textEdit.textCursor ().deletePreviousChar();
                    break;
                case Qt::Key_P://选择
                    if(!ui->listWidget->isHidden () && ui->listWidget->count () > 0)
                    {
                        textEdit.insertPlainText (ui->listWidget->currentItem ()->text ());
                        pinyin_clean ();
                    }
                    break;
                case Qt::Key_C://
                    exit(0);
                    break;
                case Qt::Key_Space:
                    if(clicked_rule((QPushButton*)obj))
                        break;
                default:
                    return QWidget::eventFilter(obj, event);
            }
            return true;
        }
        else if (obj->objectName ().indexOf ("pushButton") == 0)
        {
            QPushButton *pb = (QPushButton*)obj;
            if (event->type()==QEvent::FocusIn)
                pb->setStyleSheet ("background-color: rgb(114, 159, 207)");
            else if (event->type()==QEvent::FocusOut)
                pb->setStyleSheet ("");
        }
    }
    return QWidget::eventFilter(obj, event);
}

void VInput::on_listWidget_itemClicked(QListWidgetItem *item)
{
    if(!ui->listWidget->isHidden () && ui->listWidget->count () > 0)
    {
        textEdit.insertPlainText (item->text ());
        pinyin_clean ();
    }
}

void VInput::on_pushButton_tips_up_clicked()
{
    grid_jump(focusWidget (), Qt::Key_Up);
}

void VInput::on_pushButton_tips_down_clicked()
{
    grid_jump(focusWidget (), Qt::Key_Down);
}

void VInput::on_pushButton_tips_left_clicked()
{
    grid_jump(focusWidget (), Qt::Key_Left);
}

void VInput::on_pushButton_tips_right_clicked()
{
    grid_jump(focusWidget (), Qt::Key_Right);
}

void VInput::on_pushButton_tips_movL_clicked()
{
    if(!pinyin_move(false))
        textEdit.moveCursor (QTextCursor::Left);
}

void VInput::on_pushButton_tips_movR_clicked()
{
    if(!pinyin_move(true))
        textEdit.moveCursor (QTextCursor::Right);
}

void VInput::on_pushButton_tips_ent_clicked()
{
    clicked_rule((QPushButton*)focusWidget ());
}

void VInput::on_pushButton_tips_del_clicked()
{
    if(!pinyin_delete ())
        textEdit.textCursor ().deletePreviousChar();
}

void VInput::on_pushButton_tips_select_clicked()
{
    if(!ui->listWidget->isHidden () && ui->listWidget->count () > 0)
    {
        textEdit.insertPlainText (ui->listWidget->currentItem ()->text ());
        pinyin_clean ();
    }
}
