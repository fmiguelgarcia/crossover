#include "SystemMeasurement.hpp"
#include <QIODevice>
#include <QTextStream>

#include <sys/sysinfo.h>

using namespace crossOver::common;
using namespace std;

namespace
{}

SystemMeasurement::SystemMeasurement()
  : cpuLoad(0.0), freeRam(0), numProcs(0)
{}

void SystemMeasurement::serializeTo(QIODevice* dev) const
{
	QChar sp(' ');
	QTextStream out(dev);
	out << cpuLoad << sp << freeRam << sp <<  totalRam << sp <<  numProcs;
}

void SystemMeasurement::deserializeFrom(QIODevice* dev)
{
	QChar sp;
	QTextStream in(dev);
	in >> cpuLoad >> sp >> freeRam >> sp >> totalRam >> sp >> numProcs;
}

SystemMeasurement crossOver::common::makeSystemMeasurement()
{
  SystemMeasurement sm;

#if defined(__linux__)
  struct sysinfo info;
  const int sysStatus = sysinfo( &info);
  if (sysStatus == 0)
  {
    // Using 5 minute load average.
    sm.cpuLoad = (info.loads[1]  / 65536.0);
    sm.totalRam = info.totalram;
    sm.freeRam = info.freeram;
    sm.numProcs = info.procs;
  }
#endif
	return sm;
}
