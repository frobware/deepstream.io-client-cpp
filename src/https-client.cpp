#include <exception>
#include <iostream>
#include <memory>

#include <Poco/Exception.h>
#include <Poco/Net/ConsoleCertificateHandler.h>
#include <Poco/Net/FTPStreamFactory.h>
#include <Poco/Net/HTTPSStreamFactory.h>
#include <Poco/Net/HTTPStreamFactory.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/KeyConsoleHandler.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/AcceptCertificateHandler.h>
#include <Poco/Path.h>
#include <Poco/SharedPtr.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>
#include <Poco/URIStreamOpener.h>

using Poco::URIStreamOpener;
using Poco::StreamCopier;
using Poco::Path;
using Poco::URI;
using Poco::SharedPtr;
using Poco::Exception;
using Poco::Net::HTTPStreamFactory;
using Poco::Net::HTTPSStreamFactory;
using Poco::Net::FTPStreamFactory;
using Poco::Net::SSLManager;
using Poco::Net::Context;
using Poco::Net::KeyConsoleHandler;
using Poco::Net::PrivateKeyPassphraseHandler;
using Poco::Net::InvalidCertificateHandler;
using Poco::Net::ConsoleCertificateHandler;
int main(int argc, char** argv)
{
    HTTPStreamFactory::registerFactory();
    HTTPSStreamFactory::registerFactory();

    if (argc != 2) {
        Path p(argv[0]);
        std::cerr << "usage: " << p.getBaseName() << " <uri>" << std::endl;
        return 1;
    }
    try {
	URI u (argv[1]);
	
        Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> pAcceptCertHandler = new Poco::Net::AcceptCertificateHandler(true);
        Poco::Net::Context::Ptr pContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED, 9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");

        SSLManager::instance().initializeClient(NULL, pAcceptCertHandler, pContext);

        Poco::Net::HTTPSClientSession theSess(u.getHost(), u.getPort ());

        // Create the HTTP request object
        Poco::Net::HTTPRequest request("POST", "/", "1.1");

        // Send the request
        std::cout << "Debug point A" << std::endl;
        std::ostream& aStream = theSess.sendRequest(request);
        std::cout << "Debug point B" << std::endl;
        if (aStream.fail()) {
            std::cout << "Fail to send HTTP request for activateEnc" << std::endl;
            return 1;
        }
    } catch (Poco::Exception& exc) {
        std::cout << "Exception caught while attempting to connect." << std::endl;
        std::cerr << "\t" << exc.displayText() << std::endl;
        return 1;
    }
    return 0;
}
