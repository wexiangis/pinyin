#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool eventFilter (QObject *obj, QEvent *event);

private slots:
    void on_toolButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
