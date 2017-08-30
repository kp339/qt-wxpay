#include <QDebug>
#include <QPixmap>
#include <QLabel>
#include <QString>
#include <QPainter>
#include <QStringList>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "http_request.h"
#include "qrencode-3.4.4/qrencode.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->wx_handle_step = 0;
    this->waring_handle_step = 0;
    this->scan_left_time = 120;
    this->money.clear();
    this->start_to_pay = false;
    this->timer = new QTimer();
    this->timer_waring = new QTimer();
    connect(this->timer, SIGNAL(timeout()), this, SLOT(wx_handler()));
    connect(this->timer_waring, SIGNAL(timeout()), this, SLOT(waring_handler()));
    this->timer->start(1000);
    this->timer_waring->start(2000);
    QPixmap pixmap_scan("scan.png");
    QPixmap pixmap_success("success.png");

    ui->label_scan->setGeometry(10, 160, 280, 280);
    ui->label_scan->setPixmap(pixmap_scan);
    ui->label_success->setGeometry(200, 20, 400, 120);
    ui->label_success->setPixmap(pixmap_success);
    ui->label_success->setVisible(false);
    ui->label_waring->setHidden(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 调用qrencode函数直接生成二维码图片
void MainWindow::generateQRcode(QString qrcodeStr)
{
    QRcode *qrcode;
    //QR_ECLEVEL_Q 容错等级
    qrcode = QRcode_encodeString(qrcodeStr.toAscii().data(), 2, QR_ECLEVEL_Q, QR_MODE_8, 1);
    ui->qrcode_img->setGeometry(300, 140, 200, 200);
    int temp_width = ui->qrcode_img->width();
    int temp_height = ui->qrcode_img->height();

    int qrcode_width = qrcode->width;
    double scale_x = (double)temp_width / (double)qrcode_width;
    double scale_y =(double) temp_height /(double) qrcode_width;
    
    QImage mainimg = QImage(temp_width,temp_height,QImage::Format_RGB16);
    QPainter painter(&mainimg);
    QColor background(Qt::white);
    painter.setBrush(background);
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, 0, temp_width, temp_height);
    QColor foreground(Qt::black);
    painter.setBrush(foreground);
    for( int y = 0; y < qrcode_width; y ++) {
        for(int x = 0; x < qrcode_width; x++) {
            unsigned char b = qrcode->data[y * qrcode_width + x];
            if(b & 0x01) {
                QRectF r(x * scale_x, y * scale_y, scale_x, scale_y);
                painter.drawRects(&r, 1);
            }
        }
    }
    QPixmap mainmap = QPixmap::fromImage(mainimg);
    ui->qrcode_img->setPixmap(mainmap);
    ui->qrcode_img->setVisible(true);
}

void MainWindow::wx_handler()
{
    switch (wx_handle_step) {
    case 0:
    {
        if (this->start_to_pay == true) {
            this->wx_handle_step = 1;
            this->start_to_pay = false;
            disable_keyboard();
        }
        timer->start(500);
        break;
    }
    case 1:
    {
        QString pay_money = "money=" + money;
        QString post_ret = Post("http://192.168.1.164:8080/example/native.php", pay_money);
        QStringList strlist = post_ret.split("###");
        if (strlist.size() != 2)
            break;
        this->wx_query_url = "out_trade_no=" + strlist.at(0);
        this->wx_pay_url = strlist.at(1);

        QString cmd = "qrencode " + wx_pay_url + " -s 6 -o /opt/qrcode.png";
        int ret = system(cmd.toAscii().data());
        if (ret == 0) {
            // 调用工具方式
            system("sync");
            QPixmap pixmap_qrcode("/opt/qrcode.png");
            ui->qrcode_img->setGeometry(300, 140, 200, 200);
            ui->qrcode_img->setPixmap(pixmap_qrcode);
            ui->qrcode_img->setVisible(true);
        } else {
            // 直接调动源码方式
            generateQRcode(wx_pay_url);
        }

        

        this->wx_handle_step = 2;
        this->timer->start(5000);
        this->scan_left_time = 120;
        QString scan_left_time_str = "二维码生成成功，扫码剩余时间 : " + QString::number(scan_left_time, 10);
        this->ui->label_waring->setText(tr(scan_left_time_str.toAscii().data()));
        this->ui->label_waring->setHidden(false);
        this->waring_handle_step = 3;
        this->timer_waring->start(1000);
        break;
    }
    case 2:
    {
        QString pay_status = Post("http://192.168.1.164:8080/example/orderquery.php", wx_query_url).toUpper();
        qDebug() << "pay_status " << pay_status;
        if (pay_status.compare("SUCCESS") == 0) {
            this->wx_handle_step = 0;
            this->waring_handle_step = 0;
            this->ui->label_waring->setHidden(true);
            this->ui->label_money->setText("0");
            this->ui->label_success->setVisible(true);
            this->money.clear();
            enable_keyboard();
            this->timer->start(500);
        }
        this->timer->start(3000);
        break;
    }
    };
}

void MainWindow::waring_handler()
{
    switch (this->waring_handle_step) {
    case 0:
        break;
    case 1:
        this->waring_handle_step = 2;
        this->ui->label_waring->setHidden(false);
        this->timer_waring->start(3000);
        break;
    case 2:
        this->waring_handle_step = 0;
        this->ui->label_waring->setHidden(true);
        break;
    case 3:
        if (this->scan_left_time == 0) {
            this->ui->label_waring->setText(tr("扫码超时，请重新操作"));
            this->ui->qrcode_img->setVisible(false);
            this->wx_handle_step = 0;
            this->waring_handle_step = 1;
            this->money.clear();
            this->ui->label_money->setText("0");
            enable_keyboard();
        } else {
            this->scan_left_time--;
            QString scan_left_time_str = "二维码生成成功，扫码剩余时间 : " + QString::number(scan_left_time, 10);
            this->ui->label_waring->setText(tr(scan_left_time_str.toAscii().data()));
        }
        break;

    }
}
void MainWindow::set_money()
{
    if (!this->money.isEmpty()) {
        if (this->money.size() > 5) {
            this->ui->label_waring->setText(tr("金额超出范围!"));
            this->waring_handle_step = 1;
            this->money.remove(money.size() - 1);
            this->waring_handle_step = 1;
            this->timer_waring->start(20);
            return;
        }
        this->ui->label_money->setText(this->money);
    } else {
        this->ui->label_money->setText("0");
    }

}

void MainWindow::disable_keyboard()
{
    ui->pushButton_0->setEnabled(false);
    ui->pushButton_1->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->pushButton_4->setEnabled(false);
    ui->pushButton_5->setEnabled(false);
    ui->pushButton_6->setEnabled(false);
    ui->pushButton_7->setEnabled(false);
    ui->pushButton_8->setEnabled(false);
    ui->pushButton_9->setEnabled(false);
    ui->pushButton_ok->setEnabled(false);
    ui->pushButton_del->setEnabled(false);
}

void MainWindow::enable_keyboard()
{
    ui->pushButton_0->setEnabled(true);
    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    ui->pushButton_6->setEnabled(true);
    ui->pushButton_7->setEnabled(true);
    ui->pushButton_8->setEnabled(true);
    ui->pushButton_9->setEnabled(true);
    ui->pushButton_ok->setEnabled(true);
    ui->pushButton_del->setEnabled(true);
}
void MainWindow::on_pushButton_1_clicked()
{
    this->money.append("1");
    set_money();
}

void MainWindow::on_pushButton_2_clicked()
{
    this->money.append("2");
    set_money();
}

void MainWindow::on_pushButton_3_clicked()
{
    this->money.append("3");
    set_money();
}

void MainWindow::on_pushButton_4_clicked()
{
    this->money.append("4");
    set_money();
}

void MainWindow::on_pushButton_5_clicked()
{
    this->money.append("5");
    set_money();
}

void MainWindow::on_pushButton_6_clicked()
{
    this->money.append("6");
    set_money();
}

void MainWindow::on_pushButton_7_clicked()
{
    this->money.append("7");
    set_money();
}

void MainWindow::on_pushButton_8_clicked()
{
    this->money.append("8");
    set_money();
}

void MainWindow::on_pushButton_9_clicked()
{
    this->money.append("9");
    set_money();
}

void MainWindow::on_pushButton_0_clicked()
{
    if (!this->money.isEmpty())
        this->money.append("0");
    set_money();
}

void MainWindow::on_pushButton_ok_clicked()
{
    this->ui->label_success->setVisible(false);
    if (!this->money.isEmpty())
        this->start_to_pay = true;
}

void MainWindow::on_pushButton_del_clicked()
{
    this->money.clear();
    set_money();
}
