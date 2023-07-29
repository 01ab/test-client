
#ifndef ICLIENT_H
#define ICLIENT_H

#include <QObject>

class IClient : public QObject {
    Q_OBJECT

public:
    explicit IClient(QObject* parent = nullptr)
        : QObject { parent } {};

public slots:
    virtual void send(QByteArray data) = 0;
    virtual bool connected() = 0;
signals:
    void receive(QByteArray data);
    void error(QString text);
};

#endif // ICLIENT_H
