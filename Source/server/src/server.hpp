#pragma once
#include <QObject>
#include <QByteArray>

namespace crossOver
{
	namespace server
	{

		class SimpleHttpServer;
		class Server
			: public QObject
		{
				Q_OBJECT
			public:
				Server( QObject *parent = nullptr );

			private:
				void setupTcpServer();
				void setupDBDefaultConnection();
				void initializeTablesIfNotExist();
				void setupDB();
				void deserializePayLoad( QByteArray data);

			private:
				SimpleHttpServer *m_httpServer;
		};
	}
}
