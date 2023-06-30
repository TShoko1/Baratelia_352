#include "server.h"

#include <QDebug>
#include <QRegularExpression>
#include <QStringList>

struct Product {
    QString name;
    QString type;
    int price;
    QString manufacturer;
};

QList<Product> products = {
    {"Товар1", "Тип1", 967, "ApexAssembly"},
       {"Товар2", "Тип2", 868, "UniversalMakers"},
       {"Товар3", "Тип3", 884, "FutureFactories"},
    {"Товар4", "Тип4", 888, "TechSolutions"},
    {"Товар5", "Тип5", 600, "SuperiorSuppliers"},
    {"Товар6", "Тип6", 838, "PremierProducts"},
    {"Товар7", "Тип7", 760, "QualityCraft"},

       {"Товар100", "Тип10", 988, "ParamountParts"}
};

Server::Server(QObject *parent) : QObject(parent)
{
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::newConnection);

    if (!tcpServer->listen(QHostAddress::Any, 33333)) {
        qDebug() << "Unable to start the server:" << tcpServer->errorString();
        return;
    }

    qDebug() << "Server started. Listening on port 33333...";
}

Server::~Server()
{
    tcpServer->close();
}



void Server::newConnection()
{
    QTcpSocket *client = tcpServer->nextPendingConnection();
    connect(client, &QTcpSocket::readyRead, this, &Server::readyRead);

    buffers.insert(client, QByteArray());
    qDebug() << "New client connected:" << client->peerAddress().toString();
}

void Server::readyRead()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (!client)
        return;

    QByteArray buffer = client->readAll();
    buffers[client] += buffer;

    while (buffers[client].contains('\n')) {
        int endIndex = buffers[client].indexOf('\n');
        QString command = buffers[client].left(endIndex).trimmed();
        buffers[client] = buffers[client].mid(endIndex + 1);

        QString response = processCommand(command);
        sendResponse(client, response);
    }
}

QString Server::processCommand(const QString& command)
{
    QStringList parts = command.split('&');

    if (parts.isEmpty())
        return "Invalid command.";

    QString action = parts[0].toLower();

    if (action == "max_price") {
        // Найти самые дорогие товары
        int maxPrice = 0;
        QList<QString> maxPriceProducts;

        for (const Product& product : products) {
            if (product.price > maxPrice) {
                maxPrice = product.price;
                maxPriceProducts.clear();
                maxPriceProducts.append(product.name);
            } else if (product.price == maxPrice) {
                maxPriceProducts.append(product.name);
            }
        }

        QString response = "Max price products:\n";
        for (const QString& productName : maxPriceProducts) {
            response += productName + "\n";
        }

        return response;

    } else if (action == "expensive_in_type") {
        if (parts.size() != 2)
            return "Invalid command. Usage: expensive_in_type&type";

        QString type = parts[1];
        int totalPrice = 0;
        int totalCount = 0;

        for (const Product& product : products) {
            if (product.type == type) {
                totalPrice += product.price;
                totalCount++;
            }
        }

        if (totalCount == 0)
            return "No products of type '" + type + "' found.";

        int averagePrice = totalPrice / totalCount;

        QList<QString> expensiveProducts;
        for (const Product& product : products) {
            if (product.type == type && product.price > averagePrice)
                expensiveProducts.append(product.name);
        }

        QString response = "Expensive products in type '" + type + "':\n";
        for (const QString& productName : expensiveProducts) {
            response += productName + "\n";
        }

        return response;

    } else if (action == "max_price_in_type") {
        if (parts.size() != 2)
            return "Invalid command. Usage: max_price_in_type&type";

        QString type = parts[1];
        int maxPrice = 0;
        QList<QString> maxPriceProducts;

        for (const Product& product : products) {
            if (product.type == type && product.price > maxPrice) {
                maxPrice = product.price;
                maxPriceProducts.clear();
                maxPriceProducts.append(product.name);
            } else if (product.type == type && product.price == maxPrice) {
                maxPriceProducts.append(product.name);
            }
        }

        QString response = "Max price products in type '" + type + "':\n";
        for (const QString& productName : maxPriceProducts) {
            response += productName + "\n";
        }

        return response;

    } else if (action == "expensive") {
        int totalPrice = 0;
        int totalCount = 0;

        for (const Product& product : products) {
            totalPrice += product.price;
            totalCount++;
        }

        if (totalCount == 0)
            return "No products found.";

        int averagePrice = totalPrice / totalCount;

        QList<QString> expensiveProducts;
        for (const Product& product : products) {
            if (product.price > averagePrice)
                expensiveProducts.append(product.name);
        }

        QString response = "Expensive products:\n";
        for (const QString& productName : expensiveProducts) {
            response += productName + "\n";
        }

        return response;

    } else {
        return "Unknown command.";
    }

    return "Command executed successfully.";
}


void Server::sendResponse(QTcpSocket* client, const QString& response)
{
    QByteArray data = response.toUtf8() + '\n';
    client->write(data);
}

