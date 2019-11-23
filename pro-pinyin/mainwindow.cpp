#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "vinput.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setGeometry (0, 0, this->width (), this->height ());
    //
    ui->pushButton_all->installEventFilter (this);
    ui->pushButton_capital->installEventFilter (this);
    ui->pushButton_low->installEventFilter (this);
    ui->pushButton_number->installEventFilter (this);
    ui->pushButton_pinyin->installEventFilter (this);
    ui->pushButton_symbol->installEventFilter (this);
    ui->pushButton_word->installEventFilter (this);
    ui->pushButton_user->installEventFilter (this);
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
                case Qt::Key_Z:
                    close ();
                    break;
                case Qt::Key_C:
                    exit(0);
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
                pb->setStyleSheet ("background-color: rgb(0, 0, 255)");
            else if (event->type()==QEvent::FocusOut)
                pb->setStyleSheet ("");
        }
    }
    return QWidget::eventFilter(obj, event);
}

void MainWindow::on_pushButton_number_clicked()
{
    VInput *vk = new VInput(&result, VInput::KB_NUMBER);
    vk->exec ();
}
void MainWindow::on_pushButton_low_clicked()
{
    VInput *vk = new VInput(&result, VInput::KB_LOWER);
    vk->exec ();
}
void MainWindow::on_pushButton_capital_clicked()
{
    VInput *vk = new VInput(&result, VInput::KB_CAPITAL);
    vk->exec ();
}
void MainWindow::on_pushButton_word_clicked()
{
    VInput *vk = new VInput(&result, VInput::KB_LOWER|VInput::KB_CAPITAL);
    vk->exec ();
}
void MainWindow::on_pushButton_pinyin_clicked()
{
    VInput *vk = new VInput(&result, VInput::KB_PINYIN);
    vk->exec ();
}
void MainWindow::on_pushButton_symbol_clicked()
{
    VInput *vk = new VInput(&result, VInput::KB_SYMBOL);
    vk->exec ();
}
void MainWindow::on_pushButton_all_clicked()
{
    VInput *vk = new VInput(&result, VInput::KB_ANY);
    vk->exec ();
}
void MainWindow::on_pushButton_user_clicked()
{
    VInput *vk = new VInput(&result,
        VInput::KB_LOWER|
        VInput::KB_NUMBER|
        VInput::KB_USER, ".-:/");
    vk->exec ();
}
