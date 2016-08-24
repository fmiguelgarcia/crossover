/*
 * Copyright (c) 2016 Francisco Miguel Garcia <miguel_garcia@programmingresearch.com>
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

#include "ClientConfiguration.h"
#include <QFile>
#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

using namespace crossOver::server;
using namespace std;

namespace {
  void loadAlert( ClientAlert& ca, QDomElement alertElem)
  {
    QString type = alertElem.attribute("type");
    QString limit = alertElem.attribute("limit", "-1");

    if (type == "memory")
      ca.memoryLimit = limit.toDouble();
    else if (type == "cpu")
      ca.cpuLimit = limit.toDouble();
    else if (type == "processes")
      ca.processesLimit = limit.toUInt();
    else
      qWarning() <<  "XD£X";
  }

  ClientConfiguration loadClientNode( QDomElement clientElem)
  {
    ClientConfiguration cc;

    cc.key = clientElem.attribute("key");
    cc.mail = clientElem.attribute("mail");

    QDomNodeList alertNodeList = clientElem.elementsByTagName("alert");
    for (int i = 0; i < alertNodeList.size(); ++i)
      loadAlert( cc.alerts, alertNodeList.at(i).toElement());

    return cc;
  }
}

vector< ClientConfiguration > loadClientConfFromXml( QString filePath)
{
  vector<ClientConfiguration> ccList;

  // 1. Load DOM document
  QFile f(filePath);
  if (!f.open(QIODevice::ReadOnly))
    qFatal("XXX");

  QDomDocument doc("clientConf");
  if (!doc.setContent(&f))
    qFatal( "YYY");
  f.close();

  QDomElement docElem = doc.documentElement();
  QDomNode clientNode = docElem.firstChild();
  while ( !clientNode.isNull())
  {
    ClientConfiguration cc = loadClientNode( clientNode.toElement());
    ccList.push_back(cc);
    clientNode = clientNode.nextSibling();
  }

  return ccList;
}
