#include <fmt/format.h>
#include <fmt/ostream.h>

#include "chrome_remote/browser.h"

namespace {
// clang-format off
static constexpr auto chromium_args = 
	"--headless "
	"--disable-gpu "
	"--disable-translate "
	"--disable-extensions "
	"--disable-background-networking "
	"--safebrowsing-disable-auto-update "
	"--disable-sync "
	"--metrics-recording-only "
	"--disable-default-apps "
	"--no-first-run "
	"--mute-audio "
	"--hide-scrollbars "
	"--remote-debugging-port=9009";
// clang-format on
} // namespace

namespace chrome_remote {

Browser::Browser(ba::io_service& io, std::string_view exe)
	: ap{io}
	, proc{fmt::format("{} {}", exe, chromium_args),
		  bp::std_in.close(), bp::std_out > bp::null,
		  bp::std_err > ap}
{
}
} // namespace chrome_remote

// vim: ts=4:sts=0:noet:sw=0
