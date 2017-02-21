
#include <exception>
#include <iostream>
#include <memory>

#include <Poco/Exception.h>
#include <Poco/Net/ConsoleCertificateHandler.h>
#include <Poco/Net/FTPStreamFactory.h>
#include <Poco/Net/HTTPSStreamFactory.h>
#include <Poco/Net/HTTPStreamFactory.h>
#include <Poco/Net/KeyConsoleHandler.h>
#include <Poco/Net/SSLManager.h>
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

    // Note: we must create the passphrase handler prior to Context.

    // Ask the user via console for YES/NO to accept the certificate
    SharedPtr<InvalidCertificateHandler> ptrCert = new ConsoleCertificateHandler(false);

    Context::Ptr ptrContext = new Context(Context::CLIENT_USE,
					  "", 
					  "",
					  "", 
					  Context::VERIFY_NONE, 
					  9, 
					  false,
					  "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");

    // Use SSL with no cert, no checks.
    
    SSLManager::instance().initializeClient(0, 0, 0);

    try {
        URI uri(argv[1]);
        std::unique_ptr<std::istream> pStr(URIStreamOpener::defaultOpener().open(uri));
        StreamCopier::copyStream(*pStr.get(), std::cout);
    } catch (Exception& exc) {
        std::cerr << exc.displayText() << std::endl;
        return 1;
    }

    return 0;
}
