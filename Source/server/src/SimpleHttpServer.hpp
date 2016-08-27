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
namespace crossOver
{
	namespace server
	{
		/// @brief This class is used to represent the HTTP process context.
		struct HttpRequestProc
		{
			QTcpSocket *client;									///< Client socket.
			QString realm;											///< Client authentication key
			std::map<QString, QString> headers; ///< HTTP headers
			QByteArray content;									///< HTTP payload

			/// @return It returns true if all headers have been yet read.
			bool isHeaderFinished () const;

			/// @return It returns true if the HTTP payload has been read.
			bool isContentReady () const;
		};

		/// @brief This class makes the abstraction of a Simple HTTP server.
		/// It uses HTTP Basic Authentication.
		class SimpleHttpServer : public QObject
		{
			Q_OBJECT
		public:
			explicit SimpleHttpServer (QObject *parent = nullptr);

			/// @brief It authorizes the @p realm as a valid user-password pair.
			void addBasicAuthentication (const QString &realm);

		signals:
			void payLoadReady (QString realm, QByteArray data);

		private slots:
			/// @brief It proceccess new connections asynchronously.
			void onNewConnection ();

			/// @brief Whenever data is available on socket, this function process
			///  the content of the new data.
			void onReadyRead (std::shared_ptr<HttpRequestProc> rp);

			/// @brief It checks the authentication. It authentication is not valid,
			/// a http 401 request is generated and sent to client.
			///  If the authorization is valid, this function will emit @c
			///  payLoadReady signal.
			void processRequest (std::shared_ptr<HttpRequestProc> rp);

		private:
			QTcpServer *m_serverSocket;							 ///< Server socket
			std::vector<QString> m_basicAuthAllowed; ///< Authorization keys.
		};
	}
}
