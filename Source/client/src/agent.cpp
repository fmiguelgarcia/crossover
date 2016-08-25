#include "agent.hpp"
#include "HttpSession.hpp"
#include <QTimer>
#include <QSettings>
#include <QDebug>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QBuffer>
#include <QString>

using namespace crossOver::client;
using namespace crossOver::common;
using namespace std;

namespace {
	inline QString debugHeader() noexcept {
		return QStringLiteral("[crossOver::client::Agent]");
	}
}

Agent::Agent(QObject *parent) : QObject(parent) {
	// load settings.
	QSettings settings;
	const uint miliSec =
			settings.value("samplePeriodOnMiliSeconds", 5 * 60 * 1000).toUInt();
	const QUrl server =
			settings.value("server", QUrl("http://127.0.0.1:8080")).toUrl();

	settings.setValue("server", server);
	settings.setValue("samplePeriodOnMiliSeconds", miliSec);
	settings.sync();

	// HttpSession
	m_serverSession = new HttpSession(server, this);

	// Setup timer
	m_sampleTimer = new QTimer(this);
	connect(m_sampleTimer, &QTimer::timeout, this, &Agent::doAndSendMeasurements);
	m_sampleTimer->start(miliSec);

	// Launch first measurement.
	doAndSendMeasurements();
}

void Agent::doAndSendMeasurements() {
	// Do measurements.
	const SystemMeasurement sm = makeSystemMeasurement();
	const QString now = QDateTime::currentDateTime().toString(Qt::ISODate);
	qDebug() << debugHeader() << "[" << now << "]"
					 << " CPU load: " << sm.cpuLoad << "%"
					 << " free RAM: " << sm.freeRam << " bytes"
					 << " total RAM: " << sm.totalRam << " bytes"
					 << " Num Processes: " << sm.numProcs;

	m_serverSession->sendMeasurement(sm);
}
