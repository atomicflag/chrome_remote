#pragma once

#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <continuable/continuable-base.hpp>
#include <regex>

namespace chrome_remote {
namespace ba = boost::asio;
namespace bp = boost::process;

class Browser {
private:
	template <typename T> void read_proc(T&& p)
	{
		ba::async_read_until(ap, buffer, '\n',
			[&, p = std::forward<T>(p)](auto ec, auto size) mutable {
				if (ec) throw boost::system::system_error{ec};
				const auto line_ptr{
					ba::buffer_cast<const char*>(buffer.data())};
				static const std::regex re{"ws://"};
				if (std::regex_search(
						line_ptr, line_ptr + size, re)) {
					p.set_value();
					return;
				}
				buffer.consume(size);
				read_proc(std::move(p));
			});
	}

public:
	Browser(ba::io_service& io, std::string_view exe = "chromium");

	auto ready()
	{
		return cti::make_continuable<void>([&](auto&& p) {
			read_proc(std::forward<decltype(p)>(p));
		});
	}

	static constexpr auto host{"127.0.0.1"};
	static constexpr auto port{"9009"};

private:
	bp::async_pipe ap;
	bp::child proc;
	ba::streambuf buffer;
};
} // namespace chrome_remote

// vim: ts=4:sts=0:noet:sw=0
