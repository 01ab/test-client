
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "entity/IClient.h"
#include <QTcpSocket>

class TcpClient : public IClient {
    Q_OBJECT

    QTcpSocket* _socket;
    QString _host;
    quint16 _port;

public:
    explicit TcpClient(QString host, int port, QObject* parent = nullptr);

    virtual void send(QByteArray data) override;
    virtual bool connected() override;
private slots:
    void read();
    void tryToConnect();
};

#endif // TCPCLIENT_H
