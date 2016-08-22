#pragma once
#include <QObject>

class QTimer;
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

			private slots:
				void doMeasurements();

			private:
				QTimer *m_sampleTimer;
				
		};
	}
}
