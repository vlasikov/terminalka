#include "port.h"
#include <qdebug.h>

Port::Port(QObject *parent) :
    QObject(parent)
{
    qDebug()<<"Port::Port";
    counterCmdRead = 0;
    counterCmdWrite = 0;
    errorRead = 0;
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

    ConnectPort();
}

void Port :: ConnectPort(void){//
    thisPort.setPortName(SettingsPort.name);
    if (thisPort.open(QIODevice::ReadWrite)) {
        if (       thisPort.setBaudRate(SettingsPort.baudRate)
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

//
void Port :: WriteToPort(QByteArray data){
    LastTx = data;

    if(thisPort.isOpen()){
        thisPort.write(data);
        pingSend =  QDateTime::currentDateTime();
        counterCmdWrite += data.length();
        infoPort(counterCmdRead, counterCmdWrite, ping);
        sqnTimerStart();                                                            // запускаем таймер для повторного отправления, при отсутствии связи

        QByteArray ba_as_hex_string_write = data.toHex();
        outPort("TX "+ba_as_hex_string_write);
    }
    else{
        outPort("ERROR: Port close.");
    }
}

//
void Port :: ReadInPort(){
    QByteArray data;
    data.append(thisPort.readAll());

    pingRead =  QDateTime::currentDateTime();
    ping = pingRead.toMSecsSinceEpoch()-pingSend.toMSecsSinceEpoch();
    qDebug()<<"ping = " + QString::number(ping);

    QByteArray ba_as_hex_string = data.toHex();

    counterCmdRead += data.length();
    infoPort(counterCmdRead, counterCmdWrite, ping);

    BufRx.append(data);
    this->Control();
}

/* Conrol read packet
 *
 */
int Port :: Control(){
    while(BufRx.length()>=3){
        qDebug()<<"Ctrl ba ="<<BufRx.toHex();
        if(BufRx[0]=='\x55'){      //ok
            if(BufRx[1]=='\x55'){
                int RxLength = (static_cast<unsigned int>(BufRx[2]) & 0xFF);

                if(RxLength <= (BufRx.length()-4)){             // bytes [0], [1], [2] and crc
                    qDebug() << "read raw=" << BufRx;
                    LastRx = BufRx;                             // запоминаем последнее сообщение
                    QByteArray ba_as_hex_string = BufRx.toHex();
                    outPort("  RX "+ba_as_hex_string);          // out string "5555010203"
                    BufRx.remove(0, RxLength+4);
                    qDebug()<<"BufRx " << BufRx;
                    return 1;
                }
                else{
                    qDebug()<<"Rx Length error";
                    return 0;                                   // Rx Length error
                }
            }
            else{
                BufRx.remove(0, 2);                             // Removes first n
                qDebug()<<"delete 0,1";
            }
        }
        else{
            BufRx.remove(0, 1);                                 // Removes first n
            qDebug()<<"delete 0";
        }
    }
    return 0;
}

void Port :: slTimer(){
    qDebug()<<"slTimer LastTx"<<LastTx.toHex();
    qDebug()<<"slTimer LastRx"<<LastRx.toHex();

    if(LastRx == LastTx){
        qDebug()<<"SendOk";
        sqnTimerStop();
    }
    else{
        qDebug()<<"errorRead=" << errorRead;
        qDebug()<<"errorReadMax=" << errorReadMax;
        errorRead++;
        WriteToPort(LastTx);

        if(errorRead>=(errorReadMax-1)){
            outPort("  RX ERROR  ");
            sqnTimerStop();
        }
    }
    LastRx.clear();
}

void Port::slParamWrite(int n){
    errorReadMax = n;
    errorRead = 0;
    qDebug()<<"errorReadMax = "<<errorReadMax;
}


