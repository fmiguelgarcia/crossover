#pragma once
#include "ClientConfiguration.hpp"
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
		class SMTPAgent;

		/// @brief This class represent the Standalone Server.
		class Server : public QObject
		{
			Q_OBJECT
		public:
			Server (QObject *parent = nullptr);

		private:
			/// @brief It initializes the simple HTTP server and start to listen
			///  for TCP connections.
			/// See documentation to know how you can change the port.
			void setupTcpServer ();

			/// @brief It initializes the database back-end.
			void setupDBEngine ();

			/// @brief It loads the client configuration XML file.
			/// The file is @c clientDefaultConf.xml and it have to be located
			///  in the same folder where the server application is executed.
			/// See documentation to know how you can change the SMTP configuration.
			void setupClientsAuthAndAlarms ();

			/// @brief It decodes the HTML payload to system measurements and
			/// makes the checks against the user's alerts.
			/// It also stores those measurements into the database and send
			/// the email notifications.
			void processStatistics (QString realm, QByteArray data);

			/// @return It returns the email associated to the specific user key.
			QString findClientMail (const QString &realm) const;

			/// @brief It verifies the measurements and the user's alarms and
			///  generate the email if it is needed.
			void checkAlertForClient (QString realm, QString findClientMail,
																common::SystemMeasurement sm);

			using ClientConfList = std::vector<ClientConfiguration>;

		private:
			SimpleHttpServer *m_httpServer; ///< HTTP Server
			DBBackEnd *m_dbBackEnd;					///< Database back-end utilities.
			SMTPAgent *m_SMTPAgent;					///< SMTP agent

			ClientConfList m_clientsConf; ///< Loaded client configuration
			QCache<QString, int>
					m_realm2ClientIdCache; ///< Cache to reduce queries to DB.
		};
	}
}
