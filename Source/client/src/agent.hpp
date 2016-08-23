#pragma once
#include "SystemMeasurement.hpp"
#include <QObject>
#include <QSemaphore>
#include <queue>
#include <mutex>
#include <memory>

class QNetworkAccessManager;
class QTimer;
class QIODevice;
namespace crossOver
{
	namespace client
	{
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
				QNetworkAccessManager *m_nam;
				QTimer *m_sampleTimer;
				std::unique_ptr<QSemaphore> m_measurementsSem;
				std::queue<crossOver::common::SystemMeasurement> m_mesurements;
				std::mutex m_mesurementsMtx;
				
		};
	}
}
