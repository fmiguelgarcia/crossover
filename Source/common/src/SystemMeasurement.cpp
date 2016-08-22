#include "SystemMeasurement.hpp"

using namespace crossOver::SystemMeasurement;
const int MAX_LOAD_AVG_SAMPLES = 3;


SystemMeasurement makeSystemMeasurement()
{
	// Load cpu 
	double loadAvg[MAX_LOAD_AVG_SAMPLES];	
	const int nProc = getloadavg( loadAvg, MAX_LOAD_AVG_SAMPLES);

	return {0, loadAvg[0], 0};
}
