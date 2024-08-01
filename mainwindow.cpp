#include "mainwindow.h"
#include <QMessageBox>
#include <QString>
#include <QDesktopWidget>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>
#include <unistd.h>
#include <errno.h>
#include "ui_mainwindow.h"
#include <QThread>
#include <qdebug.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->BaudRateBox, SIGNAL(currentIndexChanged(int)) ,this, SLOT(checkCustomBaudRatePolicy(int)));
    ui->BaudRateBox->addItem(QLatin1String("57600"), QSerialPort::Baud57600);
    ui->BaudRateBox->addItem(QLatin1String("9600"), QSerialPort::Baud9600);
    ui->BaudRateBox->addItem(QLatin1String("19200"), QSerialPort::Baud19200);
    ui->BaudRateBox->addItem(QLatin1String("38400"), QSerialPort::Baud38400);
    ui->BaudRateBox->addItem(QLatin1String("115200"), QSerialPort::Baud115200);
    ui->BaudRateBox->addItem(QLatin1String("Custom"));
   // fill data bits
    ui->DataBitsBox->addItem(QLatin1String("5"), QSerialPort::Data5);
    ui->DataBitsBox->addItem(QLatin1String("6"), QSerialPort::Data6);
    ui->DataBitsBox->addItem(QLatin1String("7"), QSerialPort::Data7);
    ui->DataBitsBox->addItem(QLatin1String("8"), QSerialPort::Data8);
    ui->DataBitsBox->setCurrentIndex(3);
   // fill parity
    ui->ParityBox->addItem(QLatin1String("None"), QSerialPort::NoParity);
    ui->ParityBox->addItem(QLatin1String("Even"), QSerialPort::EvenParity);
    ui->ParityBox->addItem(QLatin1String("Odd"), QSerialPort::OddParity);
    ui->ParityBox->addItem(QLatin1String("Mark"), QSerialPort::MarkParity);
    ui->ParityBox->addItem(QLatin1String("Space"), QSerialPort::SpaceParity);
   // fill stop bits
    ui->StopBitsBox->addItem(QLatin1String("1"), QSerialPort::OneStop);
    #ifdef Q_OS_WIN
    ui->StopBitsBox->addItem(QLatin1String("1.5"), QSerialPort::OneAndHalfStop);
    #endif
    ui->StopBitsBox->addItem(QLatin1String("2"), QSerialPort::TwoStop);
   // fill flow control
    ui->FlowControlBox->addItem(QLatin1String("None"), QSerialPort::NoFlowControl);
    ui->FlowControlBox->addItem(QLatin1String("RTS/CTS"), QSerialPort::HardwareControl);
    ui->FlowControlBox->addItem(QLatin1String("XON/XOFF"), QSerialPort::SoftwareControl);
    connect(ui->cBtnSend,SIGNAL(clicked()),this, SLOT(on_cEnterText_returnPressed()) );
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QThread *thread_New = new QThread;//Создаем поток для порта платы
    Port *PortNew = new Port();//Создаем обьект по классу
    PortNew->moveToThread(thread_New);//помешаем класс  в поток
    PortNew->thisPort.moveToThread(thread_New);//Помещаем сам порт в поток
    connect(PortNew, SIGNAL(error_(QString)), this, SLOT(Print(QString)));//Лог ошибок
    connect(thread_New, SIGNAL(started()), PortNew, SLOT(process_Port()));//Переназначения метода run
    connect(PortNew, SIGNAL(finished_Port()), thread_New, SLOT(quit()));//Переназначение метода выход
    connect(thread_New, SIGNAL(finished()), PortNew, SLOT(deleteLater()));//Удалить к чертям поток
    connect(PortNew, SIGNAL(finished_Port()), thread_New, SLOT(deleteLater()));//Удалить к чертям поток
    connect(this,SIGNAL(savesettings(QString,int,int,int,int,int)),PortNew,SLOT(Write_Settings_Port(QString,int,int,int,int,int)));//Слот - ввод настроек!
    connect(ui->BtnConnect, SIGNAL(clicked()),PortNew,SLOT(ConnectPort()));
    connect(ui->BtnDisconect, SIGNAL(clicked()),PortNew,SLOT(DisconnectPort()));
    connect(PortNew, SIGNAL(outPort(QString)), this, SLOT(Print(QString)));//Лог ошибок ? ЗАЧЕМ ?
    connect(this,SIGNAL(writeData(QByteArray)),PortNew,SLOT(WriteToPort(QByteArray)));

    connect(PortNew, SIGNAL(infoPort(int, int, int)), this, SLOT(sl_infoPort(int, int, int)));
    thread_New->start();
}
//++++++++[Процедура закрытия приложения]+++++++++++++++++++++++++++++++++++++++++++++
MainWindow::~MainWindow()
{

    delete ui; //Удаление формы
}

//++++++++[Процедура определения подключенных портов]+++++++++++++++++++++++++++++++++++
void MainWindow::on_Btn_Serch_clicked()
{
    ui->PortNameBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
        ui->PortNameBox->addItem(info.portName());
        }
}

void MainWindow::checkCustomBaudRatePolicy(int idx)
{
    bool isCustomBaudRate = !ui->BaudRateBox->itemData(idx).isValid();
    ui->BaudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->BaudRateBox->clearEditText();
    }
}

//+++++++++++++[Процедура ввода данных из строки]++++++++++++++++++++++++++++++++++++++++
void MainWindow::on_cEnterText_returnPressed()
{
    QByteArray ba;                                // Текстовая переменная
    ba = ui->cEnterText->text().toLocal8Bit();    // Присвоение "data" значения из EnterText
    qDebug()<< "ba="<<ba;

//    char *baC = ba.data();
//    QString str0 = ba.data();
//    qDebug()<< "str0="<<str0;
//    QByteArray ba0 = str0.toUtf8();
//    qDebug()<< "ba0="<<ba0;

//    QStringList strList = str0.split(QRegExp(","));
//    qDebug() << "strList=" <<  strList;
//    QByteArray  baList  = str0.split(QRegExp(","));

//    bool ok;
//    QString str;
//    QByteArray ba3;
//    qDebug() << "foreach";
//    foreach (str, strList)
//    {
//        qDebug() << str.toUInt(&ok, 16);
//        //ba3.append(str.toUtf8().toHex());
//        str = "55";
//        ba3.append(str);
//    }
//    qDebug()<< "ba3="<<ba3.data();

    QByteArray text = QByteArray::fromHex(ba);
//    qDebug() <<"text="<< text.data();            // returns "Qt is great!"

//    if (!ok) {
//        qDebug() << "Parsing failed, handle error here";
//    }

//    QByteArray data = QByteArray::fromHex(ui->cEnterText->text().toUtf8().toHex());
//    qDebug() <<"data="<< data;

//    QString str1 = "\x55\x55\x01\x01\x03";

//    qDebug()<< "str1="<<str1;
//    QByteArray ba1 = str1.toUtf8();
//    qDebug()<< "ba1="<<ba1;
//    QByteArray ba2 = QByteArray::fromRawData(ba1.data(), 5);
//    qDebug()<< "ba2="<<ba2.data();

    writeData(text); // Отправка данных в порт
//    Print(ba1);     // Вывод данных в консоль


}
//+++++++++++++[Процедура вывода данных в консоль]++++++++++++++++++++++++++++++++++++++++
void MainWindow::Print(QString data)
{
    ui->consol->textCursor().insertText(data+'\r'); // Вывод текста в консоль
    ui->consol->moveCursor(QTextCursor::End);       //Scroll
}

void MainWindow::on_BtnSave_clicked()
{

savesettings(ui->PortNameBox->currentText(), ui->BaudRateBox->currentText().toInt(),ui->DataBitsBox->currentText().toInt(),
             ui->ParityBox->currentText().toInt(), ui->StopBitsBox->currentText().toInt(), ui->FlowControlBox->currentText().toInt());

}

void MainWindow::on_cBtnSend_clicked()
{

}

void MainWindow::sl_infoPort(int counterPacketRead, int counterPacketWrite, int ping)
{
    ui->label_4->setText(QString::number(counterPacketRead));
    ui->label_2->setText(QString::number(counterPacketWrite));
    ui->label_6->setText(QString::number(ping));
}

// сохранить положение
void MainWindow::on_pushButton_clicked()
{
//    QString s = "\x55\x55\x01\x01\x02";
    QByteArray ba = QByteArray::fromRawData("\x55\x55\x01\x01\x02", 5);
    writeData(ba); // Отправка данных в порт
}

// номер канала
void MainWindow::on_pushButton_2_clicked()
{
    QByteArray ba(6, 0);
    ba[0] = 0x55;
    ba[1] = 0x55;
    ba[2] = 0x02;
    ba[3] = 0x02;
    ba[4] = ui->spinBox->value();
    ba[5] = ba[2]+ba[3]+ba[4];
    writeData(ba); // Отправка данных в порт
}

// сохранить центр
void MainWindow::on_pushButton_3_clicked()
{
    QByteArray ba = QByteArray::fromRawData("\x55\x55\x01\x03\x04", 5);
    writeData(ba); // Отправка данных в порт
}
