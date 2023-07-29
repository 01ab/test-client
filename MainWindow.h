
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QJsonDocument>
#include <QMainWindow>
#include <QTreeWidgetItem>

#include <entity/IClient.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

    IClient* _client;

public:
    MainWindow(IClient* client, QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void sync();
    void receiveData(QByteArray data);

private:
    Ui::MainWindow* ui;
    QJsonDocument requestData();
    void addRootEntry(QJsonObject data);
    int getOrCreateColumn(QString name);
    void addFields(QTreeWidgetItem *entry, QJsonObject data);
};

#endif // MAINWINDOW_H
