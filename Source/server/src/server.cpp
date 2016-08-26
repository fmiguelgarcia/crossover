#include "server.hpp"
#include "SystemMeasurement.hpp"
#include "SimpleHttpServer.hpp"
#include "DBBackEnd.hpp"
#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QBuffer>
#include <QDebug>

using namespace crossOver::server;
using namespace crossOver::common;

namespace
{
	inline QString debugHeader() noexcept
	{
		return QStringLiteral("[crossOver::server::Server]");
	}
}

Server::Server(QObject *parent) : QObject(parent)
{
  // Set up cache.
  QSettings settings;
  const int clientCachedSize = settings.value( "clientCachedSize", 200).toInt();
  m_realm2ClientIdCache.setMaxCost( clientCachedSize);

	m_dbBackEnd = new DBBackEnd( this);
	setupClientsAuthAndAlarms();
	setupTcpServer();
}

void Server::setupClientsAuthAndAlarms()
{
	QFileInfo xmlCCPath(QDir(QCoreApplication::applicationDirPath()),
											"clientDefaultConf.xml");
	m_clientsConf = loadClientConfFromXml(xmlCCPath.absoluteFilePath());
}

void Server::setupTcpServer()
{
	m_httpServer = new SimpleHttpServer(this);
	connect(m_httpServer, &SimpleHttpServer::payLoadReady, this,
					&Server::processStatistics);
	for ( const auto & cc : m_clientsConf)
		m_httpServer->addBasicAuthentication( cc.key);
}

void Server::processStatistics(QString realm, QByteArray data)
{
	SystemMeasurement sm;

	// 1. Deserialize stats from HTTP payload
	QBuffer buffer(&data);
	buffer.open(QIODevice::ReadOnly);
	sm.deserializeFrom(&buffer);

	qDebug() << debugHeader() << "Cpu: " << sm.cpuLoad
					 << " Free Ram: " << sm.freeRam << " Procs: " << sm.numProcs;

  // 2. Save stats into DB.
	m_dbBackEnd->addStatsToClient( realm, sm);

	// 3. Check alarms and send email.
}

int main(int argc, char *argv[])
{
	// Configure app.
	QCoreApplication app(argc, argv);
	QCoreApplication::setOrganizationName("CrossOver");
	QCoreApplication::setApplicationName("server");

	QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope,
										 QCoreApplication::applicationDirPath());
	Server *server = new Server(&app);
	Q_UNUSED(server);

	return app.exec();
}
