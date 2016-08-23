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
#include <memory>

using namespace crossOver::server;
using namespace std;

namespace {
	inline QString HttpResponse200() noexcept
	{ return QStringLiteral("HTTP-1.1 200 OK\r\n\r\n");}
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
	if (!rp->isHeaderFinished())
	{
		// Read headers
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

	// Read content
	rp->content.append( rp->client->readAll());

	// Check
	if (rp->isContentReady())
	{
		qDebug() << "HTTP:" << "Data (" << rp->content.size() <<  "): " << rp->content;
		rp->client->write( HttpResponse200().toUtf8().constData());
		rp->client->flush();
		rp->client->waitForBytesWritten();
		rp->client->disconnectFromHost();
		emit payLoadReady( rp->content);
		rp.reset();
	}
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

