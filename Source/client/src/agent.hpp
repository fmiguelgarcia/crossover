#pragma once
#include "SystemMeasurement.hpp"
#include <QObject>
#include <queue>

class QTimer;
namespace crossOver
{
	/// @brief It is the client agent.
	/// It makes samples every 5 minutes and send to the server.
	namespace client
	{
		class HttpSession;
		class Agent : public QObject
		{
			Q_OBJECT
		public:
			Agent (QObject *parent = nullptr);

		signals:
			void finished ();
			void tryToSendMeasurements ();

		private:
			void sendMeasurement (const QByteArray data);
			void doAndSendMeasurements ();

		private:
			QTimer *m_sampleTimer;				///< Sample timer
			HttpSession *m_serverSession; ///< HTTP client session
		};
	}
}
