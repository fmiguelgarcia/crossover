#include "agent.hpp"
#include <QTimer>
#include <QSettings>
#include <QDebug>
#inc
lude <QDateTime>

using namespace crossOver::client;

Agent::Agent( QObject *parent )
	:QObject( parent)
{
	// load settings.
	QSettings settings;
	const uint miliSec = settings.value( "samplePeriodOnMiliSeconds", 5 * 60 * 1000).toUInt();

	// Setup timer
	m_sampleTimer = new QTimer( this);
	connect(	m_sampleTimer, &QTimer::timeout, this, &Agent::doMeasurements);
	m_sampleTimer->start( miliSec);

	// Launch first measurement.
	doMeasurements();
}

void Agent::doMeasurements()
{
	qDebug() << "crossOver::client::Agent is taking a sample at " << QDateTime::currentDateTime().toString( Qt::ISODate);

}
