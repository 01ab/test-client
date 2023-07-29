
#include "TcpClient.h"

TcpClient::TcpClient(QString host, int port, QObject* parent)
    : IClient { parent }
    , _host { host }
    , _port { (quint16)port }
{
    _socket = new QTcpSocket;
    connect(_socket, &QTcpSocket::readyRead, this, &TcpClient::read);
    connect(_socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) {
        emit TcpClient::error(_socket->errorString());
        tryToConnect();
    });
    tryToConnect();
}

void TcpClient::tryToConnect()
{
    _socket->connectToHost(_host, _port);
}

void TcpClient::send(QByteArray data)
{
    if (!connected()) {
        emit error("Try to send data when socket is disconnected");
        return;
    }
    _socket->write(data);
}

bool TcpClient::connected()
{
    return _socket->state() == QAbstractSocket::SocketState::ConnectedState;
}

void TcpClient::read()
{
    QByteArray data = _socket->readAll();
    while (_socket->waitForReadyRead(10)) {
        data.append(_socket->readAll());
    }
    emit receive(data);
}
