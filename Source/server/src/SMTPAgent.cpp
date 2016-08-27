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
#include "SMTPAgent.hpp"
#include <QDebug>
#include <QDateTime>
#include <QTextStream>
#include <QRegExp>

using namespace crossOver::server;
using namespace std;

namespace
{
	struct upload_status
	{
		int linesRead = 0;
		QStringList bodyLines;
	};

	static size_t payload_source(void *ptr, size_t size, size_t nmemb,
															 void *userp)
	{
		struct upload_status *upload_ctx = (struct upload_status *)userp;

		if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1))
			return 0;

		if ( upload_ctx->bodyLines.size() > upload_ctx->linesRead)
		{
			const QString line = upload_ctx->bodyLines[ upload_ctx->linesRead++];
			const QByteArray lineUTF8 = line.toUtf8();
			const int lineUTF8Size = lineUTF8.size();
			memcpy(ptr, lineUTF8, lineUTF8Size);

			return lineUTF8Size;
		}

		return 0;
	}

	QStringList generatePayLoad( const QString& from, const QStringList& toList, const QString& subject, const QString& body)
	{
		QStringList payload;
		QDateTime now = QDateTime::currentDateTime();
		QString newLine ("\r\n");

		// Date field, format:
		//    Date: Mon, 29 Nov 2010 21:54:29 +1100\r\n"
		payload << QString( "Date: %1\r\n").arg( now.toString( "ddd, dd MMM yyyy hh:mm:ss"));
		for (const QString& to: toList)
			payload << QString("To: %1\r\n").arg( to);
		payload << QString( "From: %1\r\n").arg( from);
		payload << QString( "Subject: %1\r\n").arg( subject);

		// Emtpy line to divide headers from body,  see RFC5322
		payload << newLine << body << newLine;

		return payload;
	}
}

SMTPAgent::SMTPAgent(const QString &mailServerUrl, QObject *parent)
		: QObject(parent), m_mailServer(mailServerUrl)
{
	m_curl = shared_ptr<CURL>( curl_easy_init(), &curl_easy_cleanup);
}

bool SMTPAgent::send(const QString &from, const QStringList &toList,
										 const QString &subject, const QString &message) const
{
	CURLcode res = CURLE_OK;
	if (m_curl)
	{
		CURL *curl = m_curl.get();
		struct curl_slist *recipients = nullptr;
		struct upload_status upload_ctx;
		upload_ctx.bodyLines = generatePayLoad( from, toList, subject, message);

		// User and pass
		curl_easy_setopt(curl, CURLOPT_USERNAME, m_user.toLocal8Bit().constData());
		curl_easy_setopt(curl, CURLOPT_PASSWORD, m_password.toLocal8Bit().constData());

		// Mailserver
		curl_easy_setopt(curl, CURLOPT_URL, m_mailServer.toLocal8Bit().constData());

		// TLS
		curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
		/// @bug Disable verification of valid certificate.
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		// From
		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from.toLocal8Bit().constData());

		// Recipients: We will use just "to:"
		for (const QString &to : toList)
			recipients = curl_slist_append(recipients, to.toLocal8Bit().constData());
		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

		// Callback for read body.
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
		curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
		{
			m_lastErrorCode = res;
			m_lastErrorMessage = curl_easy_strerror(res);
			qWarning() << "[SMTP Agent] It can NOT send the email: "
								 << m_lastErrorMessage;
		}

		// Cleaning
		curl_slist_free_all(recipients);
	}

	return res == CURLE_OK;
}

void SMTPAgent::setUser(const QString& user)
{ m_user = user; }

void SMTPAgent::setPassword(const QString& password)
{ m_password = password; }

int SMTPAgent::lastErrorCode() const noexcept
{ return m_lastErrorCode; }

QString SMTPAgent::lastErrorMessage() const noexcept
{ return m_lastErrorMessage; }
