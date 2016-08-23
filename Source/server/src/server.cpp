#include "server.hpp"
#include <QCoreApplication>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QTcpServer>
#include <QDebug>
#include <QtGlobal>

using namespace crossOver::server;

Server::Server( QObject *parent)
  : QObject(parent)
{
  QSettings settings;
  const quint16 port = settings.value("PORT", 8080).toUInt();

  // m_nam = new QNetworkAccessManager(this);
  m_tcpServer = new QTcpServer(this);
  connect( m_tcpServer, &QTcpServer::newConnection, this, &Server::onNewConnection);

  if ( !m_tcpServer->listen( QHostAddress::Any, port))
  {
    qFatal("[crossOver::server::Server] It can not listen on port %d", port);
  }
}

void Server::onNewConnection()
{
  QTcpSocket * client = m_tcpServer->nextPendingConnection();
  connect (client, &QAbstractSocket::disconnected, client, &QObject::deleteLater);

  /// @todo Process

  client->disconnectFromHost();
}

int main( int argc, char *argv[])
{
	// Configure app.
	QCoreApplication app( argc, argv);
	QCoreApplication::setOrganizationName("CrossOver");
	QCoreApplication::setApplicationName("server");

	QSettings::setPath( QSettings::NativeFormat, QSettings::SystemScope, QCoreApplication::applicationDirPath());
	Server *server = new Server(&app);

	return app.exec ();
}

