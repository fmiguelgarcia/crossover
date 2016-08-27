#pragma once
#include <QtGlobal>

class QIODevice;
namespace crossOver
{
	namespace common
	{
		/// @brief A common structure shared by clients and
		/// server.
		struct SystemMeasurement
		{
			SystemMeasurement ();

			/// @brief It serializes the content of this objet into a device.
			void serializeTo (QIODevice *dev) const;

			/// @brief It deserializes from a device.
			void deserializeFrom (QIODevice *dev);

			double cpuLoad;		///< 5 minutes CPU load.
			quint64 totalRam; ///< Total ram.
			quint64 freeRam;	///< Total free ram
			quint64 numProcs; ///< Number of running process
		};

		/// @brief It makes a system measurement.
		SystemMeasurement makeSystemMeasurement ();
	}
}
