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

namespace crossOver
{
	namespace common
	{
		struct SystemMeasurement;
	}
	namespace server
	{
		/// @brief This abstraction resolves the access to the database.
		class DBBackEnd : public QObject
		{
			Q_OBJECT
		public:
			explicit DBBackEnd (QObject *parent = nullptr);

			/// @brief It gets the client Identifier from @p realm (its authentication
			/// key).
			/// @return It returns the clientId, or -1 if it is not found.
			int findClientIdByRealm (const QString &realm) const;

			/// @brief It creates and return the new client id.
			int createClientId (const QString &realm, const QString &mail) const;

			/// @brief It adds the statistic on a specific user.
			int
			addStatsToClient (const QString &realm, const QString &email,
												const crossOver::common::SystemMeasurement &sm) const;

		private:
			/// @brief It defines the default database connection.
			void setupDBDefaultConnection ();

			/// @brief It creates the database Schema if it does not exit yet.
			void initializeTablesIfNotExist ();
		};
	}
}
