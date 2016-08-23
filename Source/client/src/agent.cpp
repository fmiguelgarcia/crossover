#include "agent.hpp"
#include "SystemMeasurement.hpp"
#include <QTimer>
#include <QSettings>
#include <QDebug>
#include <QDateTime>

using namespace crossOver::client;
using namespace crossOver::common;

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
	SystemMeasurement sm = makeSystemMeasurement();
  qDebug() << "[crossOver::client::Agent][" << QDateTime::currentDateTime().toString( Qt::ISODate) << "]"
          << " CPU load: " << sm.cpuLoad << "%"
          << " free RAM: " << sm.freeRam << " bytes"
          << " total RAM: " << sm.totalRam <<  " bytes"
          << " Num Processes: " << sm.numProcs;

}
