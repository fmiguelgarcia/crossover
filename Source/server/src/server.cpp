#include "server.hpp"
#include "AppLogger.hpp"
#include "ServerSettings.hpp"
#include "SystemMeasurement.hpp"
#include "SimpleHttpServer.hpp"
#include "DBBackEnd.hpp"
#include "SMTPAgent.hpp"
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
	/// @brief Functor to sort containers of @c ClientConfiguration elements
	///  by their @c key field.
	struct RealmLessThanOnClientConfiguration                  // clang-format off
	{
		bool operator()(const ClientConfiguration &lh, const ClientConfiguration &rh) const noexcept
		{ return lh.key < rh.key; }
	};                                                         // clang-format on


	/// @brief It generates the list of messages when a system measurement @p sm
	/// exceds the configuration @p cc.
	QStringList generateAlertMessages (const ClientConfiguration &cc,
																		 const SystemMeasurement &sm)
	{
		QStringList alertMessages;

		if (sm.cpuLoad > cc.alerts.cpuLimit)
			alertMessages
					<< QString ("CPU alert: Current load is %1 %%, threshold = %2 %%)")
								 .arg (sm.cpuLoad)
								 .arg (cc.alerts.cpuLimit);

		const double freeRamPerc = (sm.freeRam * 100.0) / sm.totalRam;
		if (freeRamPerc < cc.alerts.memoryLimit)
			alertMessages << QString ("Free memory alert: Current free memory is %1 "
																"%% (%2), threshold = %3 %%)")
													 .arg (freeRamPerc)
													 .arg (sm.freeRam)
													 .arg (cc.alerts.memoryLimit);

		if (sm.numProcs > cc.alerts.processesLimit)
			alertMessages << QString ("Number of running processes alert: Current "
																"number of running processes is %1,  threshold "
																"= %2")
													 .arg (sm.numProcs)
													 .arg (cc.alerts.processesLimit);

		return alertMessages;
	}
}

Server::Server (QObject *parent) : QObject (parent)
{

	setupDBEngine();
	setupClientsAuthAndAlarms ();
	setupTcpServer ();
}

void Server::setupDBEngine ()
{
	qDebug() << ServerLog::header() << "Initializing database engine";
	m_dbBackEnd = new DBBackEnd (this);
}

void Server::setupClientsAuthAndAlarms ()
{
	qDebug() << ServerLog::header() << "Loading clients and alarms";

	//  Load Alarms and client conf
	QFileInfo xmlCCPath (QDir (QCoreApplication::applicationDirPath ()),
											 "clientDefaultConf.xml");
	m_clientsConf = loadClientConfFromXml (xmlCCPath.absoluteFilePath ());
	sort (begin (m_clientsConf), end (m_clientsConf),
				RealmLessThanOnClientConfiguration ());

	QSettings settings;
	const int clientCachedSize = settings.value (ServerSettings::clientCacheSize(), 200).toInt ();
	m_realm2ClientIdCache.setMaxCost (clientCachedSize);

	// Settup SMTP agent
	const QString smtpServer = settings.value( ServerSettings::smtpServer(), "smtp://smtp.gmail.com:587").toString();
	const QString smtpUser = settings.value( ServerSettings::smtpUser(), "crossoveru1@gmail.com").toString();
	const QString smtpPass = settings.value( ServerSettings::smtpPassword(), "oCaQNY8vJvqAdHOi8W5n4Q").toString();

	m_SMTPAgent = new SMTPAgent ( smtpServer, this);
	m_SMTPAgent->setUser( smtpUser);
	m_SMTPAgent->setPassword( smtpPass);
}

void Server::setupTcpServer ()
{
	qDebug() << ServerLog::header() << "Opening HTTP port and authorization keys";

	m_httpServer = new SimpleHttpServer (this);
	connect (m_httpServer, &SimpleHttpServer::payLoadReady, this,
					 &Server::processStatistics);
	for (const auto &cc : m_clientsConf)
		m_httpServer->addBasicAuthentication (cc.key);
}

void Server::processStatistics (QString realm, QByteArray data)
{
	SystemMeasurement sm;

	// 1. Deserialize stats from HTTP payload
	QBuffer buffer (&data);
	buffer.open (QIODevice::ReadOnly);
	sm.deserializeFrom (&buffer);

	qDebug () << ServerLog::header() << "Cpu: " << sm.cpuLoad
						<< " Free Ram: " << sm.freeRam << " Procs: " << sm.numProcs;
	const QString email = findClientMail (realm);

	// 2. Save stats into DB.
	m_dbBackEnd->addStatsToClient (realm, email, sm);

	// 3. Check alarms and send email.
	checkAlertForClient (realm, email, sm);
}

QString Server::findClientMail (const QString &realm) const
{
	QString email;
	const ClientConfiguration ccFindKey (realm);

	const auto itrRange =
			equal_range (begin (m_clientsConf), end (m_clientsConf), ccFindKey,
									 RealmLessThanOnClientConfiguration ());
	if (itrRange.first != itrRange.second)
		email = itrRange.first->mail;

	return email;
}

void Server::checkAlertForClient (QString realm, QString findClientMail,
																	SystemMeasurement sm)
{
	const ClientConfiguration ccFindKey (realm);

	const auto itrRange =
			equal_range (begin (m_clientsConf), end (m_clientsConf), ccFindKey,
									 RealmLessThanOnClientConfiguration ());
	if (itrRange.first != itrRange.second)
	{
		const ClientConfiguration &cc = *itrRange.first;
		const QStringList alertMessages = generateAlertMessages (cc, sm);

		if (!alertMessages.empty ())
		{
			QString emailBody;
			QTextStream eb(&emailBody);

			eb << "System has found the following alarms:" << endl;
			for (const QString& alert: alertMessages)
				eb << "\t" << alert <<  endl;

			m_SMTPAgent->send( m_SMTPAgent->user(), QStringList(cc.mail), QStringLiteral("[ALARMS] CrossOver Notification Deamon"),
								emailBody);
		}
	}
}

int main (int argc, char *argv[])
{
	// Configure app.
	QCoreApplication app (argc, argv);
	QCoreApplication::setOrganizationName ("CrossOver");
	QCoreApplication::setApplicationName ("server");
	QSettings::setPath (QSettings::NativeFormat, QSettings::SystemScope,
											QCoreApplication::applicationDirPath ());

	// Settup logger
	AppLogger::initialize( QFileInfo( QDir(QCoreApplication::applicationDirPath()), "server.log"));

	// Create server instance
	Server *server = new Server (&app);
	Q_UNUSED (server);

	return app.exec ();
}
