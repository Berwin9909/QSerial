#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringList>
#include <QTableView>
#include <cmath>
#include <string>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial=new QSerialPort(this);
    connect(serial,SIGNAL(readyRead()),this,SLOT(readData()));
    setup();
}

MainWindow::~MainWindow()
{
    serial->close();
    delete ui;
}

void MainWindow::readData()
{
    QRegExp delim(",*");
    QStringList gpsstring;
    QString gpssentence;
    while(serial->canReadLine())
    {
        QByteArray data=serial->readLine();
        ui->tbDataRaw->setText(QString(data.trimmed()));

        if(!ui->ckRawView->isChecked()==true)
        {
            ui->gpRawData->hide();
            ui->tvSatView->hide();
            MainWindow::resize(813,314);
        }


        else
        {
            ui->gpRawData->show();
            ui->tvSatView->show();
            MainWindow::resize(1030,620);
        }

        gpssentence=data.trimmed();
        gpsstring=gpssentence.split(",");

        if(gpsstring.value(0)=="$GPGGA")
        {
            //ui->textBrowser1->append(gpsstring.value(0));
            GPGGA(gpsstring);
        }

        if(gpsstring.value(0)=="$GPRMC")
            GPRMC(gpsstring);

        if(gpsstring.value(0)=="$GPGSV")
            GPGSV(gpsstring);

        if(gpsstring.value(0)=="$GPGSA")
            GPGSA(gpsstring);




        /* QString str = "Bob 1, 2, 3";

    QRegExp rx("[, ]");// match a comma or a space
    QStringList list = str.split(rx, QString::SkipEmptyParts); */


    }
}

void MainWindow::GPGGA(QStringList str)
{
    setlocale(LC_NUMERIC,"en_US");
    double latdec,latdeg,latmin,latsec,alt;
    double londec,londeg,lonmin,lonsec;
    double lattemp1,lontemp1,Geoid;

    int satno;
    int hour,min,sec;
    int lhour;

    int tz,dst;

    QString latTemp,lonTemp,fixtype;

    latdeg=(str.value(2).mid(0,2).toInt());
    latmin=(str.value(2).mid(2,2).toInt());
    latsec=(str.value(2).mid(4,6).toDouble())*60;

    latdec=latdeg+(latmin/60)+(latsec/3600);
    if(str.value(3)=="S")
        latdec=-latdec;

    ui->tblatDeg->setText(QString::number(latdeg));
    ui->tblatMin->setText(QString::number(latmin));
    ui->tblatSec->setText(QString::number(latsec,'f',4));
    ui->tblatDec->setText(QString::number(latdec,'f',4));

    londeg=(str.value(4).mid(0,3).toInt());
    lonmin=(str.value(4).mid(3,2).toInt());
    lonsec=(str.value(4).mid(5,6).toDouble())*60;

    londec=londeg+(lonmin/60)+(lonsec/3600);
    if(str.value(5)=="W")
        londec=-londec;

    ui->tblonDeg->setText(QString::number(londeg));
    ui->tblonMin->setText(QString::number(lonmin));
    ui->tblonSec->setText(QString::number(lonsec,'f',4));
    ui->tblonDec->setText(QString::number(londec,'f',4));

    ui->tblatHem->setText(str.value(3));
    ui->tblonHem->setText(str.value(5));

    Geoid=(str.value(11).toDouble())*3.2808;

    ui->tbGeoid->setText(QString::number(Geoid,'f',1));

    fixtype=str.value(6);

    if(fixtype=="0")
        ui->tbFix->setText("Invalid");
    if(fixtype=="1")
        ui->tbFix->setText("2D Fix");
    if(fixtype=="2")
        ui->tbFix->setText("3D fix");

    satno=(str.value(7).toInt());
    ui->tbSatno->setText(QString::number(satno));

    // ui->tbtimeUTC->setText(str.value(1).mid(0,2)+":"+str.value(1).mid(2,2)+":"+str.value(1).mid(4,2));
    hour=(str.value(1).mid(0,2)).toInt();
    min=(str.value(1).mid(2,2)).toInt();
    sec=(str.value(1).mid(4,2)).toInt();

    tz=ui->cbTZone->currentText().toInt();
    if(!ui->ckDST->isChecked()==true)
        dst=1;
    else
        dst=0;

    ui->tbtimeUTC->setText(QString::number(hour)+":"+QString::number(min)+":"+QString::number(sec));
    lhour=((hour-dst)-tz+13) % 24;
    ui->tbLTime->setText(QString::number(lhour)+":"+QString::number(min)+":"+QString::number(sec));


    ui->tbGGA->setText(str.join(","));
    alt=str.value(9).toDouble()*3.28084;  // converts the string to a double and it works!!!!

    ui->tbalt->setText(QString::number(alt,'f',1));
}

void MainWindow::GPRMC(QStringList str)
{
    QString day,month,year,magdir;
    double magvar;

    day=str.value(9).mid(0,2);
    month=str.value(9).mid(2,2);
    year=str.value(9).mid(4,2);

    ui->tbRMC->setText(str.join(","));
    ui->tbDate->setText(day+"/"+month+"/"+"20"+year);

    magvar=str.value(10).toDouble();
    magdir=str.value(11);
    ui->tbMagVar->setText(QString::number(magvar)+" "+magdir);
}

void MainWindow::GPGSV(QStringList str)
{
    int satno,SNR;

    ui->tbGSV1->update();
    ui->tbGSV2->update();
    ui->tbGSV3->update();
    ui->tbGSV4->update();

    satno=str.value(3).toInt();
    ui->tvSatView->setRowCount(satno);

    if(str.value(2)=="1")
    {
        ui->tbGSV1->setText(str.join(","));

        /*  for(row=0;row<satno,row++){
         *    for(col=0;col<=4;col++)
         * ui->tvSatView-setItem(row,col,new QTableWidgetItem(QString::number(str.value(col+4).toInt())));
         *
         * }
*/

        ui->tvSatView->setItem(0,0,new QTableWidgetItem(QString::number(str.value(4).toInt())));
        ui->tvSatView->setItem(0,1,new QTableWidgetItem(QString::number(str.value(5).toInt())));
        ui->tvSatView->setItem(0,2,new QTableWidgetItem(QString::number(str.value(6).toInt())));
        ui->tvSatView->setItem(0,3,new QTableWidgetItem(QString::number(str.value(7).toInt())));

        ui->tvSatView->setItem(1,0,new QTableWidgetItem(QString::number(str.value(8).toInt())));
        ui->tvSatView->setItem(1,1,new QTableWidgetItem(QString::number(str.value(9).toInt())));
        ui->tvSatView->setItem(1,2,new QTableWidgetItem(QString::number(str.value(10).toInt())));
        ui->tvSatView->setItem(1,3,new QTableWidgetItem(QString::number(str.value(11).toInt())));

        ui->tvSatView->setItem(2,0,new QTableWidgetItem(QString::number(str.value(12).toInt())));
        ui->tvSatView->setItem(2,1,new QTableWidgetItem(QString::number(str.value(13).toInt())));
        ui->tvSatView->setItem(2,2,new QTableWidgetItem(QString::number(str.value(14).toInt())));
        ui->tvSatView->setItem(2,3,new QTableWidgetItem(QString::number(str.value(15).toInt())));

        ui->tvSatView->setItem(3,0,new QTableWidgetItem(QString::number(str.value(16).toInt())));
        ui->tvSatView->setItem(3,1,new QTableWidgetItem(QString::number(str.value(17).toInt())));
        ui->tvSatView->setItem(3,2,new QTableWidgetItem(QString::number(str.value(18).toInt())));

        ui->tvSatView->setItem(3,3,new QTableWidgetItem(QString::number(str.value(19).mid(0,2).toInt())));



    }

    if(str.value(2)=="2")
    {
        ui->tbGSV2->setText(str.join(","));
        ui->tvSatView->setItem(4,0,new QTableWidgetItem(QString::number(str.value(4).toInt())));
        ui->tvSatView->setItem(4,1,new QTableWidgetItem(QString::number(str.value(5).toInt())));
        ui->tvSatView->setItem(4,2,new QTableWidgetItem(QString::number(str.value(6).toInt())));
        ui->tvSatView->setItem(4,3,new QTableWidgetItem(QString::number(str.value(7).toInt())));

        ui->tvSatView->setItem(5,0,new QTableWidgetItem(QString::number(str.value(8).toInt())));
        ui->tvSatView->setItem(5,1,new QTableWidgetItem(QString::number(str.value(9).toInt())));
        ui->tvSatView->setItem(5,2,new QTableWidgetItem(QString::number(str.value(10).toInt())));
        ui->tvSatView->setItem(5,3,new QTableWidgetItem(QString::number(str.value(11).toInt())));

        ui->tvSatView->setItem(6,0,new QTableWidgetItem(QString::number(str.value(12).toInt())));
        ui->tvSatView->setItem(6,1,new QTableWidgetItem(QString::number(str.value(13).toInt())));
        ui->tvSatView->setItem(6,2,new QTableWidgetItem(QString::number(str.value(14).toInt())));
        ui->tvSatView->setItem(6,3,new QTableWidgetItem(QString::number(str.value(15).toInt())));

        ui->tvSatView->setItem(7,0,new QTableWidgetItem(QString::number(str.value(16).toInt())));
        ui->tvSatView->setItem(7,1,new QTableWidgetItem(QString::number(str.value(17).toInt())));
        ui->tvSatView->setItem(7,2,new QTableWidgetItem(QString::number(str.value(18).toInt())));

        ui->tvSatView->setItem(7,3,new QTableWidgetItem(QString::number(str.value(19).mid(0,2).toInt())));


    }

    if(str.value(2)=="3")
    {
        ui->tbGSV3->setText(str.join(","));

        ui->tbGSV3->setText(str.join(","));
        ui->tvSatView->setItem(8,0,new QTableWidgetItem(QString::number(str.value(4).toInt())));
        ui->tvSatView->setItem(8,1,new QTableWidgetItem(QString::number(str.value(5).toInt())));
        ui->tvSatView->setItem(8,2,new QTableWidgetItem(QString::number(str.value(6).toInt())));
        ui->tvSatView->setItem(8,3,new QTableWidgetItem(QString::number(str.value(7).toInt())));

        ui->tvSatView->setItem(9,0,new QTableWidgetItem(QString::number(str.value(8).toInt())));
        ui->tvSatView->setItem(9,1,new QTableWidgetItem(QString::number(str.value(9).toInt())));
        ui->tvSatView->setItem(9,2,new QTableWidgetItem(QString::number(str.value(10).toInt())));
        ui->tvSatView->setItem(9,3,new QTableWidgetItem(QString::number(str.value(11).toInt())));

        ui->tvSatView->setItem(10,0,new QTableWidgetItem(QString::number(str.value(12).toInt())));
        ui->tvSatView->setItem(10,1,new QTableWidgetItem(QString::number(str.value(13).toInt())));
        ui->tvSatView->setItem(10,2,new QTableWidgetItem(QString::number(str.value(14).toInt())));
        ui->tvSatView->setItem(10,3,new QTableWidgetItem(QString::number(str.value(15).toInt())));

        ui->tvSatView->setItem(11,0,new QTableWidgetItem(QString::number(str.value(16).toInt())));
        ui->tvSatView->setItem(11,1,new QTableWidgetItem(QString::number(str.value(17).toInt())));
        ui->tvSatView->setItem(11,2,new QTableWidgetItem(QString::number(str.value(18).toInt())));

        ui->tvSatView->setItem(11,3,new QTableWidgetItem(QString::number(str.value(19).mid(0,2).toInt())));
    }

    if(str.value(2)=="4")
    {
        ui->tbGSV4->setText(str.join(","));

        ui->tbGSV4->setText(str.join(","));
        ui->tvSatView->setItem(12,0,new QTableWidgetItem(QString::number(str.value(4).toInt())));
        ui->tvSatView->setItem(12,1,new QTableWidgetItem(QString::number(str.value(5).toInt())));
        ui->tvSatView->setItem(12,2,new QTableWidgetItem(QString::number(str.value(6).toInt())));
        ui->tvSatView->setItem(12,3,new QTableWidgetItem(QString::number(str.value(7).toDouble())));

        ui->tvSatView->setItem(13,0,new QTableWidgetItem(QString::number(str.value(8).toInt())));
        ui->tvSatView->setItem(13,1,new QTableWidgetItem(QString::number(str.value(9).toInt())));
        ui->tvSatView->setItem(13,2,new QTableWidgetItem(QString::number(str.value(10).toInt())));
        ui->tvSatView->setItem(13,3,new QTableWidgetItem(QString::number(str.value(11).toDouble())));

        ui->tvSatView->setItem(14,0,new QTableWidgetItem(QString::number(str.value(12).toInt())));
        ui->tvSatView->setItem(14,1,new QTableWidgetItem(QString::number(str.value(13).toInt())));
        ui->tvSatView->setItem(14,2,new QTableWidgetItem(QString::number(str.value(14).toInt())));
        ui->tvSatView->setItem(14,3,new QTableWidgetItem(QString::number(str.value(15).toDouble())));

        ui->tvSatView->setItem(15,0,new QTableWidgetItem(QString::number(str.value(16).toInt())));
        ui->tvSatView->setItem(15,1,new QTableWidgetItem(QString::number(str.value(17).toInt())));
        ui->tvSatView->setItem(15,2,new QTableWidgetItem(QString::number(str.value(18).toDouble())));

        ui->tvSatView->setItem(15,3,new QTableWidgetItem(QString::number(str.value(19).mid(0,2).toInt())));
    }



    // ui->tbGSV1->setText(str.join(","));
}

void MainWindow::setup()
{
    QList<QSerialPortInfo> infos=QSerialPortInfo::availablePorts();
    if(infos.isEmpty())
        ui->cbPort->addItem("None");

    ui->tvSatView->setMaximumWidth(230);
    ui->tvSatView->setColumnWidth(0,45);
    ui->tvSatView->setColumnWidth(1,45);
    ui->tvSatView->setColumnWidth(2,45);
    ui->tvSatView->setColumnWidth(3,45);

    foreach(QSerialPortInfo info, infos)
    {
        ui->cbPort->addItem(info.portName());

    }
}

void MainWindow::dataWrite(QString string)
{


}

void MainWindow::GPGSA(QStringList str)
{
    double hdop,pdop,vdop;
    double systacc = 5.90544;

    ui->tbGSA->setText(str.join(","));

    hdop=(str.value(16).toDouble())*systacc;
    pdop=(str.value(15).toDouble())*systacc;
    vdop=((str.value(17).mid(0,3)).toDouble())*systacc;

    ui->tbHDOP->setText(QString::number(hdop,'f',1));
    ui->tbPDOP->setText(QString::number(pdop,'f',1));
    ui->tbVDOP->setText(QString::number(vdop,'f',1));
}

void MainWindow::on_Connect_clicked()
{
    QString Port;
    quint32 Baud;

    Port=ui->cbPort->currentText();
    Baud=ui->cbBaud->currentText().toInt();

    serial->setPortName(Port);
    serial->setBaudRate(Baud);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->open(QIODevice::ReadOnly);

}

void MainWindow::on_Quit_clicked()
{
    this->close();
}
