#include "vkeyboard.h"
#include "ui_vkeyboard.h"

#include "pinyinime.h"
#include <QDebug>

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

using namespace ime_pinyin;

void MyTextEdit::startCursor(void)
{
    QFocusEvent fe(QEvent::FocusIn, Qt::TabFocusReason);
    focusInEvent (&fe);
}

VKeyboard::VKeyboard(int type, bool space, bool multiLine, QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint),
    kb_type(type),
    useSpace(space),
    useMultiLine(multiLine),
    ui(new Ui::VKeyboard)
{
    ui->setupUi(this);
    this->setGeometry (0, 0, this->width (), this->height ());

    //---------- 拼音接口 ---------
//    if(!im_open_decoder((const char *)"./dict_pinyin.dat", (const char *)"./dict_pinyin_user.dat"))
//        qDebug() << "im_open_decoder: err!";
//    //设置输入输出上限
//    //max_sps_len: 输入拼音字母的最大长度
//    //max_hzs_len: 解码中文字符的最大长度
//    im_set_max_lens(16, 16);
//    //清除缓冲
//    im_flush_cache();
//    //
//    im_reset_search();
//    //
//    int search_res =  im_search((const char*)"zhonghaida", 10);
//    qDebug() << "search_res: " << search_res;

//    //对当前查找结果执行删除操作， 然后再重新查找
//    //pos: 拼音字母的位置或者是搜索结果ID
//    //is_pos_in_splid: 申明pos参数是字母位置还是搜索结果ID
//    //clear_fixed_this_step:
//    //返回候选数量
//    //size_t im_delsearch(size_t pos, bool is_pos_in_splid, bool clear_fixed_this_step);

//    for(int i = 0; i < search_res; i++)
//    {
//        char16 cand_str[64] = {0};
//        if(im_get_candidate(i, cand_str, 64))
//            qDebug() << "find: " << QString::fromUtf16 (cand_str);
//    }
//    im_close_decoder();

    //遍历控件,连接到eventFilter()
    QObjectList list = ui->gridLayoutWidget->children() ;
//    qDebug() << list.length()  << endl;
    foreach (QObject *obj, list)
    {
//        qDebug() << obj->objectName ();
        if(obj->objectName ().indexOf("pushButton") == 0)
            obj->installEventFilter (this);
    }
    //光标闪烁的输入行
    textEdit = new MyTextEdit();
    textEdit->setPlainText ("白日依山尽");
    textEdit->setFocusPolicy (Qt::NoFocus);
    textEdit->setLineWrapMode (QTextEdit::NoWrap);
    textEdit->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    textEdit->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    ui->verticalLayout->insertWidget (0, textEdit);
    textEdit->startCursor();
    textEdit->moveCursor (QTextCursor::EndOfLine);
    //输入字符范围限制
    if(type != ANY)
    {
        //底部栏按键使能选择
        if((type&LOWER) == 0 && (type&CAPITAL) == 0)
            ui->pushButton_30->setEnabled (false);
        if((type&NUMBER) == 0)
            ui->pushButton_29->setEnabled (false);
        if((type&PINYIN) == 0)
            ui->pushButton_31->setEnabled (false);
        if((type&SYMBOL) == 0)
            ui->pushButton_32->setEnabled (false);
        //默认键盘选择
        if(!(type&LOWER) && (type&CAPITAL))
        {
            grid_load(CAPITAL);
            ui->pushButton_30->setText ("-ABC-");
        }
        else if((type&LOWER) && !(type&CAPITAL))
        {
            grid_load(LOWER);
            ui->pushButton_30->setText ("-abc-");
        }
        else if((type&NUMBER))
        {
            grid_load(NUMBER);
        }
        else if((type&PINYIN))
        {
            grid_load(PINYIN);
        }
        else if((type&SYMBOL))
        {
            grid_load(SYMBOL);
        }
    }
    else
    {
        useSpace = true;
        useMultiLine = true;
    }
    //空格键
    ui->pushButton_20->setEnabled (useSpace);
    //回车键
    ui->pushButton_28->setEnabled (useMultiLine);
    //网格跳转属性初始化
    grid_link();
    //
    ui->widget_candidate->hide ();
}

VKeyboard::~VKeyboard()
{
    //遍历控件,销毁userData
    KeyboardGrid *kg;
    QObjectList list = ui->gridLayoutWidget->children() ;
    foreach (QObject *obj, list)
    {
        if(obj->objectName ().indexOf("pushButton") == 0)
        {
            kg = (KeyboardGrid*)(obj->userData (0));
            if(kg)
                delete kg;
        }
    }
    //
    delete ui;
}

void VKeyboard::grid_link()
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

void VKeyboard::grid_load(KB_TYPE type)
{
    QString tarString;
    switch(type)
    {
        case NUMBER:
            tarString = kb_number;
            break;
        case PINYIN:
        case LOWER:
            tarString = kb_lower;
            break;
        case CAPITAL:
            tarString = kb_capital;
            break;
        case SYMBOL:
            tarString = kb_symbol;
            break;
        default:
            return;
    }
    QPushButton *pb;
    QObjectList list = ui->gridLayoutWidget->children() ;
    int count = 0;
    foreach (QObject *obj, list)
    {
        if(obj->objectName ().indexOf("pushButton") == 0)
        {
//            qDebug() << obj->objectName ();
            pb = (QPushButton*)obj;
            if(count < tarString.size ())
            {
                pb->setText (tarString.at (count));
                pb->setEnabled (true);
            }
            else
            {
                pb->setText ("");
                pb->setEnabled (false);
            }
            count++;
            if(count > 25)
                break;
        }
    }
    ui->pushButton_20->setText ("Space");
    ui->pushButton_20->setEnabled (useSpace);
    ui->pushButton_28->setText ("Enter");
    ui->pushButton_28->setEnabled (useMultiLine);
}

void VKeyboard::grid_jump(QObject *obj, int keyType)
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
    }
}

bool VKeyboard::clicked_rule(QPushButton *pb)
{
    if(pb == ui->pushButton_20 && ui->pushButton_20->text().size ()> 1)//空格
    {
        textEdit->insertPlainText (" ");
    }
    else if(pb == ui->pushButton_28 && ui->pushButton_28->text().size () > 1)//换行
    {
        textEdit->insertPlainText ("\n");
    }
    else if(pb == ui->pushButton_29)//数字
    {
        grid_load(NUMBER);
    }
    else if(pb == ui->pushButton_30)//字母
    {
        if(ui->pushButton_30->text () == "abc")
        {
            grid_load(LOWER);
            ui->pushButton_30->setText ("ABC");
        }
        else if(ui->pushButton_30->text () == "ABC")
        {
            grid_load(CAPITAL);
            ui->pushButton_30->setText ("abc");
        }
        else
        {
            if((kb_type&CAPITAL))
            {
                grid_load(CAPITAL);
                ui->pushButton_30->setText ("-ABC-");
            }
            else// if((kb_type&LOWER))
            {
                grid_load(LOWER);
                ui->pushButton_30->setText ("-abc-");
            }
        }
    }
    else if(pb == ui->pushButton_31)//拼音
    {
        grid_load(LOWER);
    }
    else if(pb == ui->pushButton_32)//符号
    {
        grid_load(SYMBOL);
    }
    else if(pb == ui->pushButton_33)//保存结束
    {
        close ();
    }
    else if(pb == ui->pushButton_34)//取消返回
    {
        close ();
    }
    else
        textEdit->insertPlainText (pb->text ());
    textEdit->ensureCursorVisible();
    return false;
}

bool VKeyboard::eventFilter (QObject *obj, QEvent *event)
{
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
                    textEdit->moveCursor (QTextCursor::Left);
                    break;
                case Qt::Key_Minus://右移
                    textEdit->moveCursor (QTextCursor::Right);
                    break;
                case Qt::Key_Z://删除
                    textEdit->textCursor ().deletePreviousChar();
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

