#include "agent.hpp"
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
	inline QString debugHeader() noexcept
	{ return QStringLiteral("[crossOver::client::Agent]");}

	void onReplyFinished( QNetworkReply* reply, QNetworkReply::NetworkError error)
	{
		if (error != QNetworkReply::NoError)
		{
			qWarning() << debugHeader() << "It can not send measurements to server: " << reply->errorString();
		}
		// Clean.
		reply->deleteLater();
	}
}

Agent::Agent( QObject *parent )
	:QObject( parent)
{
	// load settings.
	QSettings settings;
	const uint miliSec = settings.value( "samplePeriodOnMiliSeconds", 5 * 60 * 1000).toUInt();

	m_nam = new QNetworkAccessManager(this);

	// Setup timer
	m_sampleTimer = new QTimer( this);
	connect(	m_sampleTimer, &QTimer::timeout, this, &Agent::doAndSendMeasurements);
	m_sampleTimer->start( miliSec);

	// Launch first measurement.
	doAndSendMeasurements();
}

void Agent::doAndSendMeasurements()
{
	// Do measurements.
	const SystemMeasurement sm = makeSystemMeasurement();
	const QString now = QDateTime::currentDateTime().toString( Qt::ISODate);
	qDebug() << debugHeader() << "[" << now << "]"
			<< " CPU load: " << sm.cpuLoad << "%"
			<< " free RAM: " << sm.freeRam << " bytes"
			<< " total RAM: " << sm.totalRam <<  " bytes"
			<< " Num Processes: " << sm.numProcs;

	// Serialize
	QBuffer buffer;
	if (buffer.open( QIODevice::ReadWrite))
	{
		sm.serializeTo(&buffer);
		QByteArray data = buffer.data();
		qDebug() << debugHeader() << "Serialize data(" << data.size() << ") :" << data;
		sendMeasurement( data);
	}
	else
		qCritical() << debugHeader() << "It can not serialize measurements";
}

void Agent::sendMeasurement( const QByteArray data)
{
	QSettings settings;
	QUrl sap = settings.value("SERVER_URL", "http://127.0.0.1:8080").toUrl();

	// Get a connection to the server.
	QNetworkRequest request(sap);
	request.setHeader( QNetworkRequest::UserAgentHeader,  "CrossOverClient 1.0");
	request.setHeader( QNetworkRequest::ContentTypeHeader,  "text/plain");
	request.setHeader( QNetworkRequest::ContentLengthHeader,  data.size());

	using QNetworkReplyErrorFunc = void (QNetworkReply::*)(QNetworkReply::NetworkError);

	QNetworkReply* reply = m_nam->post( request, data);
	connect( reply, &QNetworkReply::finished, this,
		[reply]() { onReplyFinished(reply, QNetworkReply::NoError); });
	connect( reply, static_cast<QNetworkReplyErrorFunc>(&QNetworkReply::error),
		this,
		[reply](QNetworkReply::NetworkError code) { onReplyFinished(reply,code);});
}
