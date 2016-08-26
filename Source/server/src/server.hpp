#pragma once
#include "ClientConfiguration.h"
#include "SystemMeasurement.hpp"
#include <QObject>
#include <QByteArray>
#include <QCache>

namespace crossOver
{
	namespace server
	{
		class SimpleHttpServer;
		class DBBackEnd;

		class Server : public QObject
		{
			Q_OBJECT
		public:
			Server(QObject *parent = nullptr);

		private:
			void setupTcpServer();
			void processStatistics(QString realm, QByteArray data);
			void setupClientsAuthAndAlarms();

    private:
      QString findClientMail( const QString &realm) const;

		private:
			SimpleHttpServer *m_httpServer;
			DBBackEnd* m_dbBackEnd;

			std::vector<ClientConfiguration> m_clientsConf;
			QCache<QString, int> m_realm2ClientIdCache;
		};
	}
}
