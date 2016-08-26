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
#include <algorithm>

using namespace crossOver::server;
using namespace crossOver::common;
using namespace std;

namespace
{
	inline QString debugHeader() noexcept
	{
		return QStringLiteral("[crossOver::server::Server]");
	}

	struct RealmLessThanOnClientConfiguration
	{
    bool operator()( const ClientConfiguration& lh, const ClientConfiguration& rh) const noexcept
    { return lh.key < rh.key;}
  };

  QStringList generateAlertMessages( const ClientConfiguration& cc, const SystemMeasurement& sm)
  {
    QStringList alertMessages;

    if ( sm.cpuLoad > cc.alerts.cpuLimit)
      alertMessages << QString("CPU alert: Current load is %1 %%, threshold = %2 %%)")
          .arg( sm.cpuLoad)
          .arg( cc.alerts.cpuLimit);

    const double freeRamPerc = (sm.freeRam * 100.0) / sm.totalRam;
    if ( freeRamPerc < cc.alerts.memoryLimit)
      alertMessages << QString("Free memory alert: Current free memory is %1 %% (%2), threshold = %3 %%)")
        .arg( freeRamPerc)
        .arg( sm.freeRam)
        .arg( cc.alerts.memoryLimit);

    if ( sm.numProcs > cc.alerts.processesLimit)
      alertMessages << QString("Number of running processes alert: Current number of running processes is %1,  threshold = %2")
        .arg( sm.numProcs)
        .arg( cc.alerts.processesLimit);

    return alertMessages;
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
	sort( begin(m_clientsConf), end(m_clientsConf), RealmLessThanOnClientConfiguration());
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
	checkAlertForClient( realm, findClientMail(realm), sm);
}

QString Server::findClientMail( const QString &realm) const
{
  QString email;
  const ClientConfiguration ccFindKey( realm);

  const auto itrRange = equal_range( begin(m_clientsConf), end(m_clientsConf), ccFindKey, RealmLessThanOnClientConfiguration());
  if ( itrRange.first != itrRange.second)
    email = itrRange.first->mail;

  return email;
}

void Server::checkAlertForClient(QString realm, QString findClientMail, SystemMeasurement sm)
{
  const ClientConfiguration ccFindKey( realm);

  const auto itrRange = equal_range( begin(m_clientsConf), end(m_clientsConf), ccFindKey, RealmLessThanOnClientConfiguration());
  if ( itrRange.first != itrRange.second)
  {
    const ClientConfiguration &cc = *itrRange.first;
    const QStringList alertMessages = generateAlertMessages( cc, sm);

    if (!alertMessages.empty())
    {
      /// @todo generate message and send it

    }
  }
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
