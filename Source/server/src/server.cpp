#include "server.hpp"
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

using namespace crossOver::server;

Server::Server( QObject *parent)
  : QObject(parent)
{
  setupDB();
  setupTcpServer();
}

void Server::setupTcpServer()
{
  QSettings settings;
  const quint16 port = settings.value("PORT", 8080).toUInt();

  m_tcpServer = new QTcpServer(this);
  connect( m_tcpServer, &QTcpServer::newConnection, this, &Server::onNewConnection);

  if ( !m_tcpServer->listen( QHostAddress::Any, port))
    qFatal("[crossOver::server::Server] It can not listen on port %d", port);
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

