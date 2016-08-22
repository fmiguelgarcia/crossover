#pragma once

namespace crossOver
{
	namespace common
	{
		struct SystemMeasurement
		{
			std::uintmax_t memory;
			std::double cpuPerc;
			std::uintmax_t runningProc;
		};

		SystemMeasurement makeSystemMeasurement();

	}
}
