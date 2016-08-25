/*
 * Copyright (c) 2016 Francisco Miguel Garcia <francisco.miguel.garcia.rodriguez@gmail.com>
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
#include "SimpleHttpServer.hpp"
#include <QSettings>
#include <QTcpServer>
#include <QTcpSocket>
#include <QStringBuilder>
#include <memory>
#include <algorithm>

using namespace crossOver::server;
using namespace std;

namespace {
	inline QString Http200Response() noexcept
	{ return QStringLiteral("HTTP-1.1 200 OK\r\n\r\n");}

	QString Http401Unauthorized ()
	{
    const QString rawContent =
      QStringLiteral("HTTP-1.1 401\r\n")
      % QStringLiteral( "WWW-Authenticate: Digest user@crossOver.com\r\n")
      % QStringLiteral( "Content-Length: 0\r\n")
      % QStringLiteral( "\r\n");

    return rawContent;
 }


  inline QString HttpHeaderAuthorization() noexcept
  { return QStringLiteral("Authorization");}

	void loadHttpHeaders( shared_ptr<HttpRequestProc> rp)
	{
    int separatorPos;
    QString headerLine;
    QString field, value;
    do {
      headerLine = rp->client->readLine();
      separatorPos = headerLine.indexOf( QChar(':'));
      field = headerLine.leftRef( separatorPos).trimmed().toString();
      value = headerLine.midRef( (separatorPos >-1)
        ? (separatorPos+1):( headerLine.size())).trimmed().toString();
      rp->headers.insert( field, value);
    }while (!rp->isHeaderFinished() && !headerLine.isEmpty());
  }

  bool isBasicAuthenticatedValid( shared_ptr<HttpRequestProc> rp,
    const vector<QString> & basicAuthRealM)
  {
    auto itr = rp->headers.find( HttpHeaderAuthorization());
    return itr != end(rp->headers)
      && binary_search( begin(basicAuthRealM), end(basicAuthRealM), itr.value());
  }

  void requestUserAuthenticate( shared_ptr<HttpRequestProc> rp)
  {
  }

  void sendHttp200(  shared_ptr<HttpRequestProc> rp)
  {
    rp->client->write( Http200Response().toUtf8().constData());
    rp->client->flush();
    rp->client->waitForBytesWritten();
    rp->client->disconnectFromHost();
  }
}

SimpleHttpServer::SimpleHttpServer(QObject* parent): QObject(parent)
{
  QSettings settings;
  const quint16 port = settings.value("PORT", 8080).toUInt();

  m_serverSocket = new QTcpServer(this);
  connect( m_serverSocket, &QTcpServer::newConnection, this, &SimpleHttpServer::onNewConnection);

  if ( !m_serverSocket->listen( QHostAddress::Any, port))
    qFatal("[crossOver::server::Server] It can not listen on port %d", port);
}

void SimpleHttpServer::onNewConnection()
{
	shared_ptr<HttpRequestProc> rp = make_shared<HttpRequestProc>();
	rp->client = m_serverSocket->nextPendingConnection();

	connect (rp->client, &QAbstractSocket::disconnected, this, [rp](){
	 rp->client->deleteLater();
	});

	connect (rp->client, &QTcpSocket::readyRead, this, [this, rp]() {
	 this->onReadyRead( rp);});
}

void SimpleHttpServer::onReadyRead( shared_ptr<HttpRequestProc> rp)
{
  // Read headers
	if (!rp->isHeaderFinished())
    loadHttpHeaders( rp);

	// Read content
	rp->content.append( rp->client->readAll());

	// Check
	if (rp->isContentReady())
	{
    processRequest( rp);
	}
}

void SimpleHttpServer::processRequest (shared_ptr< HttpRequestProc > rp)
{
  qDebug() << "HTTP:" << "Data (" << rp->content.size() <<  "): " << rp->content;

  if (!isBasicAuthenticatedValid( rp, m_basicAuthAllowed ))
  {
    requestUserAuthenticate( rp);
    return;
  }

  // Respond to client and close connection.
  sendHttp200( rp);

  // Emit payload is ready
  const QString realm = rp->headers.find( HttpHeaderAuthorization()).value();
  emit payLoadReady( realm, rp->content);
}


bool HttpRequestProc::isHeaderFinished() const
{
	return headers.contains("");
}

bool HttpRequestProc::isContentReady() const
{
	bool ok;
	const int contentLenght = headers.value("Content-Length", "").toInt(&ok);

	return ok && contentLenght == content.size();
}

void SimpleHttpServer::addBasicAuthentication (const QString &realm)
{
  if (!binary_search( begin(m_basicAuthAllowed), end(m_basicAuthAllowed),  realm))
  {
    m_basicAuthAllowed.push_back( realm);
    sort( begin(m_basicAuthAllowed), end(m_basicAuthAllowed));
  }
}


