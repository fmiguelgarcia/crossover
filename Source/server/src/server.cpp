#include "server.hpp"
#include "SystemMeasurement.hpp"
#include "SimpleHttpServer.hpp"
#include <QCoreApplication>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QTcpServer>
#include <QDebug>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QBuffer>
#include <QTimer>

using namespace crossOver::server;
using namespace crossOver::common;

namespace {
	inline QString debugHeader() noexcept
	{ return QStringLiteral( "[crossOver::server::Server]");}


}

Server::Server( QObject *parent)
  : QObject(parent)
{
  setupDB();
  setupClientsAuthAndAlarms();
  setupTcpServer();
}

void Server::setupClientsAuthAndAlarms()
{
  QFileInfo xmlCCPath( QDir(QCoreApplication::applicationDirPath()), "clientDefaultConf.xml");
  m_clientsConf = loadClientConfFromXml(xmlCCPath.absoluteFilePath());
}


void Server::setupDB()
{
  setupDBDefaultConnection();
  initializeTablesIfNotExist();
}

void Server::setupDBDefaultConnection()
{
  QSettings settings;

  // Setup DB connection.
  const QFileInfo dbFile(
    QDir(QCoreApplication::applicationDirPath()),
    "server.db");
  const QString dbName = settings.value("DBName", dbFile.absoluteFilePath()).toString();
  const QString dbDriver = settings.value("DBDriver", "QSQLITE").toString();

  QSqlDatabase db = QSqlDatabase::addDatabase(dbDriver);
  db.setDatabaseName( dbName);
  if ( !db.open())
    qFatal("[crossOver::server::Server] It can not open the database %s",
      dbFile.absoluteFilePath().toLocal8Bit().constData());
}

void Server::initializeTablesIfNotExist()
{
  // Load resource
  QFile dbSchemaInitializationScript (":/dbSchema.sql");
  if ( !dbSchemaInitializationScript.open( QIODevice::ReadOnly | QIODevice::Text))
    qFatal("[crossOver::server::Server] It can not load the database schema initialization script");
  QTextStream script( &dbSchemaInitializationScript);

  QSqlDatabase db = QSqlDatabase::database();
  QString sqlScriptLine;
  do {
    sqlScriptLine = script.readLine();
    if ( !sqlScriptLine.isNull()
          && !sqlScriptLine.isEmpty())
    {
      QSqlQuery sqlQuery = db.exec(sqlScriptLine);
      QSqlError sqlQueryError = sqlQuery.lastError();
      if (sqlQueryError.type() != QSqlError::NoError)
      {
        qFatal( "[crossOver::server::Server] Error on initialization DB schema.\nQuery: %s\n Error:%s",
          sqlScriptLine.toLocal8Bit().constData(),
          sqlQueryError.text().toLocal8Bit().constData());
      }
    }
  }while (!sqlScriptLine.isNull());
}

void Server::setupTcpServer()
{
	m_httpServer = new SimpleHttpServer( this);
	connect( m_httpServer, &SimpleHttpServer::payLoadReady, this, &Server::deserializePayLoad);
}

void Server::deserializePayLoad( QByteArray data)
{
	SystemMeasurement sm;

	QBuffer buffer(&data);
	buffer.open( QIODevice::ReadOnly);
	sm.deserializeFrom( &buffer);

	qDebug() << debugHeader() << "Cpu: " << sm.cpuLoad <<  " Free Ram: " <<  sm.freeRam << " Procs: " <<  sm.numProcs;
}



int main( int argc, char *argv[])
{
	// Configure app.
	QCoreApplication app( argc, argv);
	QCoreApplication::setOrganizationName("CrossOver");
	QCoreApplication::setApplicationName("server");

	QSettings::setPath( QSettings::NativeFormat, QSettings::SystemScope, QCoreApplication::applicationDirPath());
	Server *server = new Server(&app);
	Q_UNUSED(server);

	return app.exec ();
}

