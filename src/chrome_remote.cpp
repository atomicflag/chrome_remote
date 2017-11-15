#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

#include "chrome_remote/chrome_remote.h"

namespace chrome_remote {
namespace io = boost::iostreams;
using array_source = io::array_source;
using file_sink = io::file_descriptor_sink;
using io::copy;

ChromeRemote::ChromeRemote(ba::io_service& io)
	: socket{io}
{
}

namespace impl {
	boost::string_view to_boost(std::string_view v)
	{
		return {v.data(), v.size()};
	}

	std::string ws_addr(const std::string& str)
	{
		static const std::regex r{"ws://[^/]+(/.+)"};
		std::smatch m;
		const std::string url{
			json::parse(str)[0]["webSocketDebuggerUrl"]};
		std::regex_search(url, m, r);
		return m[1];
	}
	void save(std::string_view filename,
		const std::vector<std::uint8_t>& data)
	{
		file_sink f{
			filename.data(), std::ios::binary | std::ios::out};
		copy(array_source{reinterpret_cast<const char*>(data.data()),
				 data.size()},
			f);
	}
} // namespace impl

} // namespace chrome_remote

// vim: ts=4:sts=0:noet:sw=0
