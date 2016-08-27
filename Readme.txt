1. Instructions to install and configure prerequisites or dependencies, if any
	
	1.1. Prerequisites or dependencies.
		1. C++ 11 compiler. e.i GCC 4.9.2
		2. Qt framework 5.3.2 or greater.
		3. CMake 3.5.2 or greater
		4. libcurl-dev. I've used libcurl-openssl version from debian.
		
	1.2. Build process.
	You just need to build like another CMake project. i.e:

		$> cd crossover/Source
		$ crossover/Source> mkdir build
		$ crossover/Source> cd build
		$ crossover/Source/build >  cmake -DCMAKE_BUILD_TYPE=Debug  -DCMAKE_INSTALL_PREFIX:PATH=install  ..
		$ crossover/Source/build >  make install
		

		Files will be generated on "crossover/Source/build/install/bin".
	
	1.3. Running unit tests (SMTP agent)
	
		$ crossover/Source/build >  make test 

2. Instructions to create and initialize the database (if required)
	Not needed because the database schema is created if it does not exit.

3. Assumptions you have made - it is good to explain your thought process and the assumptions you have made
	3.1. Server requires a XML file that describes the basic authentication (key), the email that will be notified in case
	of alert, and the thresholds of those alerts.
	An example of this file is supplied ("clientDefaultConf.xml") and automatically installed into target application folder.
	
	3.2. Authentication is HTTP Basic Authentication.
	
	3.3. The SMTP Agent works with TLS BUT it does NOT verify the SMTP server certificate.
	
	3.4. The CPU load is a 5-minutes average.
	
	3.5. System has only been tested on Linux Debin 8.0, and both apps in the same machine.


4. Requirements that you have not covered in your submission, if any
	None.

5. Instructions to configure and prepare the source code to build and run properly
	Client and Server can be partially setup. In "share" directory you could see some configuration files.
	Both system expect that configuration tree is located in the same folder where application is located. That means that if
	application is located in "/home/user/proj/bin", configuration files will be loaded from "/home/user/proj/bin/CrossOver/"
	
	Client application setting file, in "<bin>/CrossOver/client.conf", you can modify the following parameters:
	
		SAMPLE_PERIOD_MILISEC: Sample period in milliseconds.
		SERVER: HTTP address for server. 
		AUTH_USER: User name that will be used on HTTP Basic Authentication.
		AUTH_PASSWORD: Password.
		
	On server side, in "<bin>/CrossOver/server.conf", you have access to:
		- SMTP_SERVER: It is the SMTP server will be used to send alert emails 
		- SMTP_USER: SMTP user, default "crossoveru1@gmail.com"
		- SMTP_PASSWORD: SMTP password, default "oCaQNY8vJvqAdHOi8W5n4Q"
		
		- CLIENT_CACHE_SIZE: It is a cache to reduce the access to the underline database 
				where we need to save the measurements.

		- HTTP_PORT: TCP port used, by default 8080.

    1.7. Issues you have faced while completing the assignment, if any

    1.8. Constructive feedback for improving the assignment


