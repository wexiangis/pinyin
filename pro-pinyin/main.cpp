#include "mainwindow.h"
#include <QApplication>

#include "pinyinime.h"
#include <QDebug>

using namespace ime_pinyin;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    if(!im_open_decoder((const char *)"./dict_pinyin.dat", (const char *)"./dict_pinyin_user.dat"))
        qDebug() << "im_open_decoder: err!";

    im_set_max_lens(16, 16);

    im_flush_cache();

    im_reset_search();

    int search_res =  im_search((const char*)"hunan", 5);
    qDebug() << "search_res: " << search_res;

    for(int i = 0; i < search_res; i++)
    {
        char16 cand_str[64] = {0};
        if(im_get_candidate(i, cand_str, 64))
            qDebug() << "find: " << QString::fromUtf16 (cand_str);
    }

    im_close_decoder();

    return a.exec();
}
