/*
 * Copyright (c) 2016 Francisco Miguel Garcia
 *<francisco.miguel.garcia.rodriguez@gmail.com>
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
#pragma once
#include <QObject>
#include <QVariant>
#include <QByteArray>
#include <memory>
#include <map>

class QTcpSocket;
class QTcpServer;
namespace crossOver {
	namespace server {
		struct HttpRequestProc {
			QTcpSocket *client;
			std::map<QString, QString> headers;
			QByteArray content;

			bool isHeaderFinished() const;
			bool isContentReady() const;
		};

		class SimpleHttpServer : public QObject {
			Q_OBJECT
		public:
			explicit SimpleHttpServer(QObject *parent = nullptr);

			void addBasicAuthentication(const QString &realm);

		signals:
			void payLoadReady(QString realm, QByteArray data);

		private slots:
			void onNewConnection();
			void onReadyRead(std::shared_ptr<HttpRequestProc> rp);
			void processRequest(std::shared_ptr<HttpRequestProc> rp);

		private:
			QTcpServer *m_serverSocket;
			std::vector<QString> m_basicAuthAllowed;
		};
	}
}
