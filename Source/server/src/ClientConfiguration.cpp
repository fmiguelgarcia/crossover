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

#include "ClientConfiguration.hpp"
#include <QFile>
#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

using namespace crossOver::server;
using namespace std;

namespace {
	void loadAlert(ClientAlert &ca, QDomElement alertElem) {
		bool memoryLimitOk = true;
		bool cpuLimitOk = true;
		bool processesLimitOk = true;

		const QString type = alertElem.attribute("type").trimmed();
		const QString limit =
				alertElem.attribute("limit", "-1").trimmed().remove("%");

		if (type == "memory")
			ca.memoryLimit = limit.toDouble(&memoryLimitOk);
		else if (type == "cpu")
			ca.cpuLimit = limit.toDouble(&cpuLimitOk);
		else if (type == "processes")
			ca.processesLimit = limit.toUInt(&processesLimitOk);
		else
			qWarning("Unknow alert type while loading client configuration file: %s",
							 type.toLocal8Bit().constData());

		if (!(memoryLimitOk && cpuLimitOk && processesLimitOk))
			qWarning("Error while loading the limit value of an alert");
	}

	ClientConfiguration loadClientNode(QDomElement clientElem) {
		ClientConfiguration cc;

		cc.key = clientElem.attribute("key");
		cc.mail = clientElem.attribute("mail");

		QDomNodeList alertNodeList = clientElem.elementsByTagName("alert");
		for (int i = 0; i < alertNodeList.size(); ++i)
			loadAlert(cc.alerts, alertNodeList.at(i).toElement());

		return cc;
	}
}

ClientConfiguration::ClientConfiguration() = default;

ClientConfiguration::ClientConfiguration( const QString& key)
  : key(key)
{}

vector<ClientConfiguration>
crossOver::server::loadClientConfFromXml(const QString& filePath) {
	vector<ClientConfiguration> ccList;
	QString errorMsg;
	int errorLine, errorColumn;

	// 1. Load DOM document
	QFile f(filePath);
	if (!f.open(QIODevice::ReadOnly))
		qFatal("XXX");

	QDomDocument doc("clientConf");
	if (!doc.setContent(&f, false, &errorMsg, &errorLine, &errorColumn)) {
		qFatal("Error parsing the client configuration at %d, %d: %s", errorLine,
					 errorColumn, errorMsg.toLocal8Bit().constData());
	}
	f.close();

	QDomElement rootElem = doc.documentElement();

	QDomNode clientNode = rootElem.firstChild();
	while (!clientNode.isNull()) {
		ClientConfiguration cc = loadClientNode(clientNode.toElement());
		ccList.push_back(cc);
		clientNode = clientNode.nextSibling();
	}

	return ccList;
}
