#pragma once
#include <QObject>

// class QNetworkAccessManager;
class QTcpServer;
namespace crossOver
{
  namespace server
  {
    class Server
      : public QObject
    {
      Q_OBJECT
      public:
        Server( QObject *parent = nullptr);

      private slots:
        void onNewConnection();
      private:
        // QNetworkAccessManager *m_nam;
        QTcpServer* m_tcpServer;
    };
  }
}
