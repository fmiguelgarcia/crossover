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
#include <QString>

namespace crossOver
{
	namespace server
	{
		class ServerSettings
		{
		public:
			static QString clientCacheSize () noexcept;
			static QString smtpServer() noexcept;
			static QString smtpUser() noexcept;
			static QString smtpPassword() noexcept;

			static QString dbDriver() noexcept;
			static QString dbName() noexcept;

		private:
			ServerSettings () = delete;
		};

		class ServerLog
		{
		public:
			static QString header() noexcept;

		private:
			ServerLog () = delete;
		};
	}
}
