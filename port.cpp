#include "port.h"
#include <qdebug.h>

Port::Port(QObject *parent) :
    QObject(parent)
{
    counterCmdRead = 0;
}

Port::~Port()
{
    qDebug("By in Thread!");
    emit finished_Port();
}

void Port :: process_Port(){
    qDebug("Hello World in Thread!");
    connect(&thisPort,SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    connect(&thisPort, SIGNAL(readyRead()),this,SLOT(ReadInPort()));
}

void Port :: Write_Settings_Port(QString name, int baudrate,int DataBits,
                         int Parity,int StopBits, int FlowControl){
    SettingsPort.name = name;
    SettingsPort.baudRate = (QSerialPort::BaudRate) baudrate;
    SettingsPort.dataBits = (QSerialPort::DataBits) DataBits;
    SettingsPort.parity = (QSerialPort::Parity) Parity;
    SettingsPort.stopBits = (QSerialPort::StopBits) StopBits;
    SettingsPort.flowControl = (QSerialPort::FlowControl) FlowControl;
}

void Port :: ConnectPort(void){//
    thisPort.setPortName(SettingsPort.name);
    if (thisPort.open(QIODevice::ReadWrite)) {
        if (thisPort.setBaudRate(SettingsPort.baudRate)
                && thisPort.setDataBits(SettingsPort.dataBits)//DataBits
                && thisPort.setParity(SettingsPort.parity)
                && thisPort.setStopBits(SettingsPort.stopBits)
                && thisPort.setFlowControl(SettingsPort.flowControl))
        {
            if (thisPort.isOpen()){
                error_((SettingsPort.name.toLocal8Bit()+ " >> Открыт!\r"));
            }
        } else {
            thisPort.close();
            error_(thisPort.errorString().toLocal8Bit());
          }
    } else {
        thisPort.close();
        error_(thisPort.errorString().toLocal8Bit());
    }
}

void Port::handleError(QSerialPort::SerialPortError error)//
{
    if ( (thisPort.isOpen()) && (error == QSerialPort::ResourceError)) {
        error_(thisPort.errorString().toLocal8Bit());
        DisconnectPort();
    }
}//


void  Port::DisconnectPort(){
    if(thisPort.isOpen()){
        thisPort.close();
        error_(SettingsPort.name.toLocal8Bit() + " >> Закрыт!\r");
    }
}
//ot tuta kovuratji!!!
void Port :: WriteToPort(QByteArray data){
    //words = QStringList::split(" ", data);
    qDebug() << "data ="<<data;
    QStringList cmd = (QString(data)).split(QRegExp("$"), QString::SkipEmptyParts);
    cmd[0] = "10";
    qDebug() << "cmd = "<<cmd;
//    if(thisPort.isOpen()){
//        thisPort.write(data);
//    }
//    timePingSend = QDateTime::currentDateTime();

    qDebug() << "cmd[0] = "<<cmd[0];
    unsigned short sh = cmd[0][0].unicode();
    qDebug() << "sh ="<<sh;
    qDebug() << "cmdList[0] ="<<cmd[0].toUShort();
    qDebug()<<"timeSend = " + timePingSend.toString("yyyy/MM/dd hh:mm:ss,zzz");

//    QString hexStr = "0123456789ABCDEF";
//    QByteArray hex = QByteArray::fromHex(hexStr.toAscii());
//    int size = hex.size();
//    for(int i = 0; i < size; ++i){
//      qDebug() << hex[i] << " ";
//    }

    QByteArray ba(5, 0); // array length 4, filled with 0
    ba[0] = 0x55;
    ba[1] = 0x55;
    ba[2] = 0x01;
    ba[3] = 0x01;
    ba[4] = 0x02;
    if(thisPort.isOpen()){
        thisPort.write(ba);
    }
    QByteArray ba_as_hex_string = ba.toHex();
    qDebug() << "ba_as_hex_string ="<<ba_as_hex_string;
}
//
void Port :: ReadInPort(){
    QByteArray data;
    data.append(thisPort.readAll());

    pingRead =  QDateTime::currentDateTime();
    qDebug()<<"Read = " + pingRead.toString("yyyy/MM/dd hh:mm:ss,zzz");
    ping = pingRead.toMSecsSinceEpoch()-timePingSend.toMSecsSinceEpoch();
    qDebug()<<"timeRead = " + QString::number(ping);

//    outPort(data);

    QByteArray ba_as_hex_string = data.toHex();
    outPort(ba_as_hex_string);

    counterCmdRead += data.length();
    infoPort(counterCmdRead, 2);
}

