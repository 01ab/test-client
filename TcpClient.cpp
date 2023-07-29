
#include "TcpClient.h"
#include <QTimer>
TcpClient::TcpClient(QString host, int port, QObject* parent)
    : IClient { parent }
    , _host { host }
    , _port { (quint16)port }
{
    _socket = new QTcpSocket;
    connect(_socket, &QTcpSocket::readyRead, this, &TcpClient::read);
    connect(_socket, &QTcpSocket::connected, this, &IClient::connectionEstablished);
    connect(_socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) {
        if (error == QAbstractSocket::SocketError::SocketTimeoutError) {
            // выпадает эта ошибка при чтении данных из сокета (точнее при ожидании новых, когда их нет в waitForReadyRead(10))
        } else {
            emit TcpClient::error(_socket->errorString());
        }
        QTimer::singleShot(10, this, &TcpClient::tryToConnect);
    });
    tryToConnect();
}

void TcpClient::tryToConnect()
{
    switch (_socket->state()) {
    case QAbstractSocket::SocketState::ConnectedState:
    case QAbstractSocket::SocketState::ConnectingState:
        // игнорим эти статусы
        break;
    default:
        _socket->connectToHost(_host, _port);
        break;
    }
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
