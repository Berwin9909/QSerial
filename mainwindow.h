#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTableView>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_Connect_clicked();
    void readData();
    void on_Quit_clicked();
    void GPGGA(QStringList str);
    void GPRMC(QStringList str);
    void GPGSV(QStringList str);
    void setup();
    void dataWrite(QString string);
    void GPGSA(QStringList str);


private:
    Ui::MainWindow *ui;
    QSerialPort *serial;


};

#endif // MAINWINDOW_H
