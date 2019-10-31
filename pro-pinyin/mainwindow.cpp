#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "pinyinime.h"
#include <QDebug>

using namespace ime_pinyin;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    QObjectList list = ui->gridLayoutWidget->children() ;
    qDebug() << list.length()  << endl;
    foreach (QObject *obj, list)
    {
        if(obj)
            qDebug() << obj->objectName ();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter (QObject *obj, QEvent *event)
{
//    if(obj->parent() == this)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            switch(keyEvent->key())
            {
                case Qt::Key_Up:
                    for(int i = 0; i < ui->gridLayout->columnCount(); i++)
                        focusNextPrevChild(false);
                    break;
                case Qt::Key_Down:
                    for(int i = 0; i < ui->gridLayout->columnCount(); i++)
                        focusNextPrevChild(true);
                    break;
                default:
                    return QWidget::eventFilter(obj, event);
            }
            return true;
        }
        else if (obj->objectName ().indexOf ("pushButtom") == 0)
        {
            QPushButton *pb = (QPushButton*)obj;
            if (event->type()==QEvent::FocusIn)
                pb->setStyleSheet ("border-image: url(:/png/res/hit.png)");
            else if (event->type()==QEvent::FocusOut)
                pb->setStyleSheet ("");
        }
    }
    return QWidget::eventFilter(obj, event);
}

