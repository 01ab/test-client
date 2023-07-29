
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>

MainWindow::MainWindow(IClient* client, QWidget* parent)
    : QMainWindow(parent)
    , _client { client }
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(_client, &IClient::error, this, [this](QString text) {
        statusBar()->showMessage(text);
        QTimer::singleShot(10000, this, &MainWindow::sync);
    });
    connect(_client, &IClient::receive, this, &MainWindow::receiveData);
    connect(_client, &IClient::connectionEstablished, this, &MainWindow::sync);

    // делаем тип первой колонкой
    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->headerItem()->setText(0, "type");

    statusBar()->showMessage("Trying to connect");
}

MainWindow::~MainWindow()
{
    delete ui;
}

QJsonDocument MainWindow::requestData()
{
    return QJsonDocument(QJsonObject { { "request", "database" } });
}

void MainWindow::sync()
{
    if (_client->connected())
        _client->send(requestData().toJson()); // отправляем запрос
}

// получаем ответ
void MainWindow::receiveData(QByteArray data)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (doc.isNull()) {
        statusBar()->showMessage(error.errorString());
        return;
    }
    if (!doc.isObject()) {
        statusBar()->showMessage("Answer is invalid ");
        return;
    }
    // транслируем жсон на QTreeView
    QJsonObject content = doc.object();
    if (!content.contains("blocks") || !content["blocks"].isArray()) {
        statusBar()->showMessage("Answer is invalid or empty");
        return;
    }
    ui->treeWidget->clear();

    // есть дерево объектов у объектов есть поля, эти поля пусть будут колонками в QTreeView
    QJsonArray blocks = content["blocks"].toArray();
    for (int i = 0; i < blocks.count(); ++i) {
        QJsonValue v = blocks[i];
        if (v.isObject())
            addRootEntry(v.toObject());
    }

    statusBar()->showMessage("Data updated successfully");
}

int MainWindow::getOrCreateColumn(QString name)
{
    QTreeWidgetItem* header = ui->treeWidget->headerItem();
    for (int i = 0; i < ui->treeWidget->columnCount(); ++i) {
        if (header->text(i) == name)
            return i;
    }
    int i = ui->treeWidget->columnCount();
    ui->treeWidget->setColumnCount(i + 1);
    header->setText(i, name);
    return i;
}

void MainWindow::addFields(QTreeWidgetItem* entry, QJsonObject data)
{
    foreach (const QString& key, data.keys()) {
        QJsonValue v = data[key];
        if (v.isString())
            entry->setText(getOrCreateColumn(key), v.toString());
        else if (v.isArray()) {
            //  тут чайлды типа
            QJsonArray a = v.toArray();
            for (int i = 0; i < a.count(); ++i) {
                QJsonValue c = a[i];
                if (c.isObject()) {
                    QTreeWidgetItem* child = new QTreeWidgetItem(entry);
                    entry->addChild(child);
                    addFields(child, c.toObject());
                }
            }
        }
    }
}

void MainWindow::addRootEntry(QJsonObject data)
{
    QTreeWidgetItem* root = new QTreeWidgetItem(ui->treeWidget);
    ui->treeWidget->addTopLevelItem(root);
    addFields(root, data);
}
