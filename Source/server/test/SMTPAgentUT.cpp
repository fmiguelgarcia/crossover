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

#include "SMTPAgentUT.hpp"
#include <QTest>
#include <QString>
#include <QStringList>


using namespace crossOver::server;

namespace {
	QString emailAccount()
	{ return QStringLiteral( "crossoveru1@gmail.com");}
}

SMTPAgentUT::SMTPAgentUT(QObject *parent) : QObject(parent)
{
	m_smtpAgent = new SMTPAgent( "smtp://smtp.gmail.com:587", this);
	m_smtpAgent->setUser( emailAccount());
	m_smtpAgent->setPassword( "oCaQNY8vJvqAdHOi8W5n4Q");
}

void SMTPAgentUT::sendEmail()
{
	QFETCH( QString, from);
	QFETCH( QStringList, toList);
	QFETCH( QString, subject);
	QFETCH( QString, message);
	QFETCH( bool, expectResult);

	QVERIFY( m_smtpAgent->send( from, toList, subject, message) == expectResult);
}

void SMTPAgentUT::sendEmail_data()
{
	QTest::addColumn<QString>("from");
	QTest::addColumn<QStringList>("toList");
	QTest::addColumn<QString>("subject");
	QTest::addColumn<QString>("message");
	QTest::addColumn<bool>("expectResult");

	QStringList uniqueTo;
	uniqueTo<< "migroslinx@gmail.com";

#if 0
	QTest::newRow("Simple 1") << emailAccount() << uniqueTo << "SMTP Simple 1" << "Hi,\nThis is an email using a three text lines.\nCheers, Miguel" << true;
	QTest::newRow("Empty body 1") << emailAccount() << uniqueTo << "SMTP body 1" << "" << true;
	QTest::newRow("Empty subject 1") << emailAccount() << uniqueTo << "" << "FYI" << true;
	QTest::newRow("Empty from 1") << "" << uniqueTo << "SMTP empty From" << "FYI" << true;
# endif

	QStringList emptyTo;
	QTest::newRow("Empty to") << emailAccount() << emptyTo << "Empty to" << "FYI" << true;
}

QTEST_MAIN(SMTPAgentUT)
