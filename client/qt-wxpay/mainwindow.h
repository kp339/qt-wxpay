#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void wx_handler();
    void waring_handler();
    
private slots:
    void on_pushButton_1_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_0_clicked();

    void on_pushButton_ok_clicked();

    void on_pushButton_del_clicked();

private:
    void set_money();
    void disable_keyboard();
    void enable_keyboard();
    Ui::MainWindow *ui;
    QTimer *timer;
    QTimer *timer_waring;
    int wx_handle_step;
    int waring_handle_step;
    int scan_left_time;
    QString money;
    bool start_to_pay;
    QString wx_pay_url;
    QString wx_query_url;
    QString wx_pay_result;
};

#endif // MAINWINDOW_H
