/*
 * Copyright (c) 2016 Francisco Miguel Garcia
 *<miguel_garcia@programmingresearch.com>
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

#include "HttpSession.hpp"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QBuffer>
#include <QSettings>

using namespace crossOver::client;

namespace
{
	void onReplyFinished(QNetworkReply *reply, QNetworkReply::NetworkError error)
	{
		if (error != QNetworkReply::NoError)
		{
			qWarning() << "It can not send measurements to server: "
								 << reply->errorString();
		}
		// Clean.
		reply->deleteLater();
	}

	void setUpAuthenticator(QAuthenticator *auth)
	{
		QSettings settings;
		const QString user = settings.value("AUTH_USER", "Aladdin").toString();
		const QString password =
				settings.value("AUTH_PASSWORD", "OpenSesame").toString();

		auth->setUser(user);
		auth->setPassword(password);
	}
}

HttpSession::HttpSession(QUrl server, QObject *parent)
		: QObject(parent), m_server(server)
{
	m_netManager = new QNetworkAccessManager(this);
	connect(m_netManager, &QNetworkAccessManager::authenticationRequired, this,
					[](QNetworkReply *reply, QAuthenticator *authenticator)
					{
		setUpAuthenticator(authenticator);
	});
	connect(m_netManager, &QNetworkAccessManager::proxyAuthenticationRequired,
					this, [](const QNetworkProxy &proxy, QAuthenticator *auth)
					{
		setUpAuthenticator(auth);
	});
}

void HttpSession::sendMeasurement(
		const crossOver::common::SystemMeasurement &sm) const
{
	// Serialize
	QBuffer buffer;
	if (buffer.open(QIODevice::ReadWrite))
	{
		sm.serializeTo(&buffer);
		QByteArray data = buffer.data();
		qDebug() << "Serialize data(" << data.size() << ") :" << data;
		sendMeasurement(data);
	}
	else
		qCritical() << "It can not serialize measurements";
}

void HttpSession::sendMeasurement(const QByteArray data) const
{
	QSettings settings;
	const QString user = settings.value("AUTH_USER", "Aladdin").toString();
	const QString password =
			settings.value("AUTH_PASSWORD", "OpenSesame").toString();
	const QString realm = QString( "%1:%2").arg( user).arg( password);

	// Get a connection to the server.
	QNetworkRequest request(m_server);
	request.setHeader(QNetworkRequest::UserAgentHeader, "CrossOverClient 1.0");
	request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
	request.setHeader(QNetworkRequest::ContentLengthHeader, data.size());
	request.setRawHeader( "Authorization", "Basic " + realm.toLocal8Bit().toBase64());

	using QNetworkReplyErrorFunc =
			void (QNetworkReply::*)(QNetworkReply::NetworkError);

	QNetworkReply *reply = m_netManager->post(request, data);
	connect(reply, &QNetworkReply::finished, this, [reply]()
					{
		onReplyFinished(reply, QNetworkReply::NoError);
	});
	connect(reply, static_cast<QNetworkReplyErrorFunc>(&QNetworkReply::error),
					this, [reply](QNetworkReply::NetworkError code)
					{
		onReplyFinished(reply, code);
	});
}
