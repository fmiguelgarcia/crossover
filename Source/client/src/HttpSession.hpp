/*
 * Copyright (c) 2016 Francisco Miguel Garcia
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
#include "SystemMeasurement.hpp"
#include <QObject>
#include <QUrl>

class QNetworkAccessManager;
namespace crossOver
{
	namespace client
	{
		/// @brief This class manages the HTTP request from client side.
		/// It adds the authorization header.
		/// User and password are loaded from configuration file.
		class HttpSession : public QObject
		{
			Q_OBJECT
		public:
			explicit HttpSession (QUrl server, QObject *parent = nullptr);

			/// @brief It serializes and send the sendMeasurement.
			void sendMeasurement (const common::SystemMeasurement &sm) const;

			/// @brief It sends the data.
			void sendMeasurement (const QByteArray) const;

		private:
			QUrl m_server;											 ///< HTTP Server
			QNetworkAccessManager *m_netManager; ///< Network manager
		};
	}
}
