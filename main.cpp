#include <boost/asio.hpp>

#include "chrome_remote/chrome_remote.h"

int main(int, char* [])
{
	boost::asio::io_service io;

	/* RAII wrapper over a headless chromium process */
	chrome_remote::Browser browser{io};

	/* Chromium devtools remote */
	chrome_remote::Remote remote{io};

	browser.ready()
		.then(remote.connect(browser))
		.then(chrome_remote::print_to_pdf(
			remote, "http://www.example.com", "example.pdf"))
		.done();

	io.run();
	return 0;
}

// vim: ts=4:sts=0:noet:sw=0
