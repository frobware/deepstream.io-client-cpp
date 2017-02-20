#include <exception>
#include <iostream>

#include <Poco/Exception.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/Net.h>
#include <Poco/URI.h>

#include <Poco/Net/NetSSL.h>
#include <Poco/Net/SSLManager.h>

struct SSLInitializer
{
	SSLInitializer() 
	{
		Poco::Net::initializeSSL();
	}
	
	~SSLInitializer()
	{
		Poco::Net::uninitializeSSL();
	}
};

int main()
{
  SSLInitializer s;

  try {
    Poco::URI uri("wss://10.69.6.74:6020/deepstream");
    Poco::Net::HTTPClientSession s("127.0.0.1");
    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, "/deepstream");
    s.sendRequest(request);
  } 
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}
