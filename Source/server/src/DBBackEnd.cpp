/*
 * Copyright (c) 2016 Francisco Miguel Garcia <miguel_garcia@programmingresearch.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "DBBackEnd.hpp"
#include "SystemMeasurement.hpp"
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QTextStream>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

using namespace crossOver::server;

DBBackEnd::DBBackEnd(QObject* parent): QObject(parent)
{
  setupDBDefaultConnection();
  initializeTablesIfNotExist();
}

void DBBackEnd::initializeTablesIfNotExist()
{
  QFile dbSchemaInitializationScript(":/dbSchema.sql");
  if (!dbSchemaInitializationScript.open(QIODevice::ReadOnly | QIODevice::Text))
    qFatal("[crossOver::server::Server] It can not load the database schema "
           "initialization script");
  QTextStream script(&dbSchemaInitializationScript);

  QSqlDatabase db = QSqlDatabase::database();
  QString sqlScriptLine;
  do
  {
    sqlScriptLine = script.readLine();
    if (!sqlScriptLine.isNull() && !sqlScriptLine.isEmpty())
    {
      QSqlQuery sqlQuery = db.exec(sqlScriptLine);
      QSqlError sqlQueryError = sqlQuery.lastError();
      if (sqlQueryError.type() != QSqlError::NoError)
      {
        qFatal("[crossOver::server::Server] Error on initialization DB "
               "schema.\nQuery: %s\n Error:%s",
               sqlScriptLine.toLocal8Bit().constData(),
               sqlQueryError.text().toLocal8Bit().constData());
      }
    }
  } while (!sqlScriptLine.isNull());
}

void DBBackEnd::setupDBDefaultConnection()
{
  QSettings settings;

  // Setup DB connection.
  const QFileInfo dbFile(QDir(QCoreApplication::applicationDirPath()),
                         "server.db");
  const QString dbName =
      settings.value("DBName", dbFile.absoluteFilePath()).toString();
  const QString dbDriver = settings.value("DBDriver", "QSQLITE").toString();

  QSqlDatabase db = QSqlDatabase::addDatabase(dbDriver);
  db.setDatabaseName(dbName);
  if (!db.open())
    qFatal("[crossOver::server::Server] It can not open the database %s",
           dbFile.absoluteFilePath().toLocal8Bit().constData());
}

int DBBackEnd::findClientIdByRealm(const QString& realm) const
{
  int id = -1;

  QSqlDatabase db = QSqlDatabase::database();
  QSqlQuery clientIdRS = db.exec(
    QString("SELECT clientId FROM client WHERE realm LIKE %1")
      .arg( realm));

  if (clientIdRS.next())
      id = clientIdRS.value(0).toInt();

  return id;
}

int DBBackEnd::createClientId( const QString& realm, const QString &mail) const
{
  QSqlDatabase db = QSqlDatabase::database();

  QSqlQuery newClientInsert = db.exec(
    QString( "INSERT INTO client (realm, email) VALUES (%1, %2)")
      .arg( realm)
      .arg( mail));

  if (newClientInsert.lastError().type() != QSqlError::NoError)
  {
    qCritical() << "Server can not add a new customer(" << mail << ") into database";
    return -1;
  }

  return newClientInsert.lastInsertId().toInt();
}

void DBBackEnd::addStatsToClient( const QString& realm, const crossOver::common::SystemMeasurement& sm) const
{
  const int clientId = findClientIdByRealm( realm);

  QSqlQuery insertSql(
      QStringLiteral( "INSERT INTO clientStats ( clientId, sampleEpoc, cpuLoad, freeRam , numRunProcs) VALUES (:clientId, now, :cpuLoad, :freeRam, :numRunProcs)"));
  insertSql.bindValue( ":clientId",  clientId);
  insertSql.bindValue( ":cpuLoad", sm.cpuLoad );
  insertSql.bindValue( ":freeRam", sm.freeRam);
  insertSql.bindValue( ":numRunProcs", sm.numProcs);

  if (!insertSql.exec())
  {
    qWarning() << "It can not insert new stats on client " << realm <<  ":" << insertSql.lastError().text();
  }
}




