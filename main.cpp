#include <boost/asio.hpp>

#include "chrome_remote/chrome_remote.h"

int main(int, char* [])
{
	boost::asio::io_service io;
	chrome_remote::ChromeRemote remote{io};

	remote.connect("127.0.0.1:9009")
		.then(chrome_remote::print_to_pdf(
			remote, "http://www.example.com", "example.pdf"))
		.done();

	io.run();
	return 0;
}

// vim: ts=4:sts=0:noet:sw=0
