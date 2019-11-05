#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "vkeyboard.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setGeometry (0, 0, this->width (), this->height ());
    ui->toolButton->installEventFilter (this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_toolButton_clicked()
{
    VKeyboard *vk = new VKeyboard(VKeyboard::LOWER|VKeyboard::PINYIN);
    vk->show ();
}

bool MainWindow::eventFilter (QObject *obj, QEvent *event)
{
    if(obj->parent() == this)
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
    }
    return QWidget::eventFilter(obj, event);
}

