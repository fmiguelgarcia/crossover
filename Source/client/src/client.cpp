#include <QCoreApplication>
#include <QSettings>
#include "agent.hpp"

using namespace crossOver::client;

int main( int argc, char *argv[])
{
	// Configure app.
	QCoreApplication app( argc, argv);
	QCoreApplication::setOrganizationName("CrossOver");
	QCoreApplication::setApplicationName("client");

	QSettings::setPath( QSettings::NativeFormat, QSettings::SystemScope, QCoreApplication::applicationDirPath());

	// Create Client agent
	Agent *agent = new Agent( &app);
	QObject::connect( agent, &Agent::finished, &app, &QCoreApplication::quit);

	return app.exec ();
}

