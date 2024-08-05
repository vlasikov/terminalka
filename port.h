#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDateTime>
#include <QTimer>
#include <QStringList>

struct Settings {
    QString name;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;
};

class Port : public QObject
{
    Q_OBJECT

public:

    explicit Port(QObject *parent = 0);

    ~Port();

    QSerialPort thisPort;
    Settings SettingsPort;
    QByteArray BufRx, LastRx;
    QByteArray LastTx;

    QDateTime pingSend, pingRead;
    qint64 ping, counterCmdRead, counterCmdWrite, errorReadMax, errorRead;
    QStringList words;

signals:

    void finished_Port(); //

    void error_(QString err);

    void outPort(QString data);

    void infoPort(int counterPacketRead, int counterPacketWrite, int ping);

    void sqnTimerStart();
    void sqnTimerStop();


private:
    int Control();

public slots:

    void DisconnectPort();

    void ConnectPort(void);

    void Write_Settings_Port(QString name, int baudrate, int DataBits, int Parity, int StopBits, int FlowControl);

    void process_Port();

    void WriteToPort(QByteArray data);

    void ReadInPort();

    void slParamWrite(int n);

private slots:

    void handleError(QSerialPort::SerialPortError error);//

    void slTimer();

};

#endif // PORT_H
