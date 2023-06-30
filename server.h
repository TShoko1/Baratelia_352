#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>

class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    ~Server();
     void slotServerRead();//.

private slots:
    void newConnection();
    void readyRead();

private:
    QTcpServer *tcpServer;
    QMap<QTcpSocket*, QByteArray> buffers;

    QString processCommand(const QString& command);

    void sendResponse(QTcpSocket* client, const QString& response);
};

#endif // SERVER_H
