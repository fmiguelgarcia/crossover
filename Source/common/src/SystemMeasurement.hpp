#pragma once
#include <QtGlobal>

class QIODevice;
namespace crossOver
{
	namespace common
	{
		struct SystemMeasurement
		{
			SystemMeasurement();

			void serializeTo( QIODevice *dev) const;
			void deserializeFrom( QIODevice *dev);

			double cpuLoad;
			quint64 totalRam;
			quint64 freeRam;
			quint64 numProcs;
		};

		SystemMeasurement makeSystemMeasurement();

	}
}
