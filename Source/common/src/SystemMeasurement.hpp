#pragma once
#include <cstdint>

namespace crossOver
{
	namespace common
	{
		struct SystemMeasurement
		{
      SystemMeasurement();

			double cpuLoad;
      std::uintmax_t totalRam;
      std::uintmax_t freeRam;
			std::uintmax_t numProcs;
		};

		SystemMeasurement makeSystemMeasurement();

	}
}
