#pragma once
#include "SystemMeasurement.hpp"
#include <QObject>
#include <queue>

class QTimer;
namespace crossOver
{
	namespace client
	{
		class HttpSession;
		class Agent : public QObject
		{
			Q_OBJECT
			public:
				Agent( QObject *parent = nullptr);

			signals:
				void finished ();
				void tryToSendMeasurements();

			private:
				void sendMeasurement( const QByteArray data);
				void doAndSendMeasurements();

			private:
				QTimer *m_sampleTimer;
				HttpSession *m_serverSession;
		};
	}
}
