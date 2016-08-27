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
#include <QString>
#include <vector>

namespace crossOver
{
	namespace server
	{
		/// @brief It stores limits.
		struct ClientAlert
		{
			double memoryLimit;	///< Free memory limit (in percentage)
			double cpuLimit;		 ///< CPU load limit.
			uint processesLimit; ///< Maximun number of running process.
		};

		/// @brief This structure links the user's alerts with its key
		/// (authorization key)
		///  and its email (used for notifications)
		struct ClientConfiguration
		{
			ClientConfiguration ();
			explicit ClientConfiguration (const QString &key);

			QString mail;				///< E-mail used to notify
			QString key;				///< Authorization token.
			ClientAlert alerts; ///< Alerts.
		};

		/// @brief Utility function to load client configuration from XML file.
		std::vector<ClientConfiguration>
		loadClientConfFromXml (const QString &filePath);
	}
}
