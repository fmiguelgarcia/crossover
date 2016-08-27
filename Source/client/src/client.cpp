#include "agent.hpp"
#include "AppLogger.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QSettings>

using namespace crossOver::client;
using namespace crossOver::common;

int main( int argc, char *argv[])
{
	// Configure app.
	QCoreApplication app( argc, argv);
	QCoreApplication::setOrganizationName("CrossOver");
	QCoreApplication::setApplicationName("client");

	QSettings::setPath( QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::applicationDirPath());

	// Settup logger
	AppLogger::initialize( QFileInfo( QDir(QCoreApplication::applicationDirPath()), "client.log"));

	// Create Client agent
	Agent *agent = new Agent( &app);
	QObject::connect( agent, &Agent::finished, &app, &QCoreApplication::quit);

	return app.exec ();
}

