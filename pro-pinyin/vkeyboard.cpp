#include "vkeyboard.h"
#include "ui_vkeyboard.h"

#include "pinyinime.h"
#include <QDebug>

using namespace ime_pinyin;

void MyTextEdit::startCursor(void)
{
    QFocusEvent fe(QEvent::FocusIn, Qt::TabFocusReason);
    focusInEvent (&fe);
}

VKeyboard::VKeyboard(QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint),
    ui(new Ui::VKeyboard)
{
    ui->setupUi(this);

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

    //---------- 控件遍历 ---------
    QObjectList list = ui->gridLayoutWidget->children() ;
    qDebug() << list.length()  << endl;
    foreach (QObject *obj, list)
    {
        if(obj->objectName ().indexOf("pushButton") == 0)
        {
//            qDebug() << obj->objectName ();
            obj->installEventFilter (this);
        }
    }

    //
    mte = new MyTextEdit();
    mte->setText ("1214524536");
    mte->setFocusPolicy (Qt::NoFocus);
    mte->setLineWrapMode (QTextEdit::NoWrap);
    mte->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    mte->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    ui->verticalLayout->insertWidget (0, mte);
    mte->startCursor();
    mte->moveCursor (QTextCursor::EndOfLine);
}

VKeyboard::~VKeyboard()
{
    delete ui;
}

void VKeyboard::jump_rule(int th, int keyType)
{
    int col = 10;
    bool isNext = false;
    //
    if(keyType == Qt::Key_Up)
    {
        if(th < 11)
        {
            if(th < 8)
                col = 7;
            else
            {
//                isNext = true;
                col = th;
            }
        }
        else if(th > 27)
            col = 7;
    }
    else if(keyType == Qt::Key_Down)
    {
        if(th > 17 && th < 21)
        {
//            ;
            isNext = true;
            col = 7 + (20 - th);
        }
        else
        {
            isNext = true;
            if(th > 20)
                col = 7;
        }
    }
    else if(keyType == Qt::Key_Left)
    {
        if(th == 1 || th == 11)
        {
            col = 9;
            isNext = true;
        }
        else if(th == 21 || th == 28)
        {
            col = 6;
            isNext = true;
        }
        else
            col = 1;
    }
    else if(keyType == Qt::Key_Right)
    {
        if(th == 10 || th == 20)
            col = 9;
        else if(th == 27 || th == 34)
            col = 6;
        else
        {
            col = 1;
            isNext = true;
        }
    }
    //
    for(int i = 0; i < col; i++)
        focusNextPrevChild (isNext);
}

bool VKeyboard::clicked_rule(QPushButton *pb)
{
    mte->append (pb->text ());
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
                    jump_rule(obj->objectName ().mid (11).toInt (), keyEvent->key());
                    break;
                case Qt::Key_Z:
                    close ();
                    break;
                case Qt::Key_C:
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

