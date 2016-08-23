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
        void setupTcpServer();
        void setupDBDefaultConnection();
        void initializeTablesIfNotExist();
        void setupDB();

      private:
        QTcpServer* m_tcpServer;
    };
  }
}
