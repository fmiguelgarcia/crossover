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
#include <QObject>
#include <QStringList>
#include <QString>
#include <curl/curl.h>
#include <memory>

namespace crossOver
{
	namespace server
	{
		/// @brief It covers the SMTP email.
		class SMTPAgent : public QObject
		{
			Q_OBJECT
		public:
			explicit SMTPAgent (const QString &mailServerUrl,
													QObject *parent = nullptr);
			SMTPAgent (const SMTPAgent &) = delete;
			SMTPAgent &operator=(const SMTPAgent &) = delete;

			/// @brief It sends the email.
			bool send (const QString &from, const QStringList &to,
								 const QString &subject, const QString &message) const;

			/// @brief It returns the last error code.
			int lastErrorCode () const noexcept;

			/// @brief It returns an human readable associate to the error.
			QString lastErrorMessage () const noexcept;

			void setUser (const QString &user);
			QString user () const;
			void setPassword (const QString &password);

		private:
			std::shared_ptr<CURL> m_curl; ///< CURL handler
			QString m_mailServer;					///< SMTP server url
			QString m_user;								///< User
			QString m_password;						///< Password

			mutable int m_lastErrorCode;
			mutable QString m_lastErrorMessage;
		};
	}
}
