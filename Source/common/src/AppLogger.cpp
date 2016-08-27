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

#include "AppLogger.hpp"
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <iostream>

using namespace crossOver::common;
using namespace std;

namespace
{
	void extMessageHandler (QtMsgType type, const QMessageLogContext &context,
													const QString &msg)
	{
		AppLogger::handleMessage (type, context, msg);
	}
}

AppLogger* AppLogger::m_self = nullptr;

void AppLogger::initialize (const QFileInfo &output)
{
	/// @bug It is NOT thread-safe.
	if (!m_self)
		m_self = new AppLogger (output, QCoreApplication::instance ());
}

AppLogger::AppLogger (const QFileInfo &output, QObject *parent)
		: QObject (parent)
{
	const QString outputPath = output.absoluteFilePath();
	m_outputFile = new QFile (outputPath, this);
	if (!m_outputFile->open (QIODevice::Append | QIODevice::Text))
		qFatal( "System can NOT create or open the log file at %s", outputPath.toLocal8Bit().constData());

	m_os.setDevice (m_outputFile);
	qInstallMessageHandler (extMessageHandler);
}

void AppLogger::handleMessage (QtMsgType type,
																const QMessageLogContext &context,
																const QString &msg)
{
	QString fMsg;
	const QString nowISO = QDateTime::currentDateTime ().toString (Qt::ISODate);
	switch (type)
	{
		case QtDebugMsg:
			fMsg = QStringLiteral ("[DEBUG]");
			break;
/*		case QtInfoMsg:
			fMsg = QStringLiteral ("[INFO]");
			break;*/
		case QtWarningMsg:
			fMsg = QStringLiteral ("[WARNING]");
			break;
		case QtCriticalMsg:
			fMsg = QStringLiteral ("[CRITICAL]");
			break;
		case QtFatalMsg:
			fMsg = QStringLiteral ("[FATAL]");
			break;
	}

	const QString finalMsg =
			QString ("%1[%2] %3").arg (fMsg).arg (nowISO).arg (msg);

	// Write down to log file.
	m_self->m_os << finalMsg << endl;
	cerr << finalMsg.toLocal8Bit ().constData () << endl;

	if (type == QtFatalMsg)
		abort ();
}
