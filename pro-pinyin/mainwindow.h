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
    void on_pushButton_number_clicked();

    void on_pushButton_low_clicked();

    void on_pushButton_capital_clicked();

    void on_pushButton_word_clicked();

    void on_pushButton_pinyin_clicked();

    void on_pushButton_symbol_clicked();

    void on_pushButton_all_clicked();

private:
    Ui::MainWindow *ui;
    QString result = "白日依山尽";
};

#endif // MAINWINDOW_H
