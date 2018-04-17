#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <continuable/continuable-base.hpp>

namespace chrome_remote::impl {
namespace ba = boost::asio;
using tcp = ba::ip::tcp;
namespace be = boost::beast;
namespace ws = be::websocket;
namespace http = be::http;

boost::string_view to_boost(std::string_view v);

inline auto connect(tcp::socket& socket, const tcp::endpoint& ep)
{
	return cti::make_continuable<void>([&](auto&& p) {
		socket.async_connect(
			ep, [p = std::forward<decltype(p)>(p)](auto ec) mutable {
				if (ec) throw boost::system::system_error{ec};
				p.set_value();
			});
	});
}

inline auto handshake(ws::stream<tcp::socket&>& socket,
	std::string_view host, std::string_view path)
{
	return cti::make_continuable<void>(
		[&, host, path = static_cast<std::string>(path)](
			auto&& p) mutable {
			socket.async_handshake(to_boost(host), path,
				[p = std::forward<decltype(p)>(p)](
					auto ec) mutable {
					if (ec) throw boost::system::system_error{ec};
					p.set_value();
				});
		});
}

template <typename T, typename B>
auto read(tcp::socket& socket, B& buffer)
{
	return cti::make_continuable<std::shared_ptr<T>>([&](auto&& p) {
		auto res = std::make_shared<T>();
		http::async_read(socket, buffer, *res,
			[&, res = std::move(res),
				p = std::forward<decltype(p)>(p)](
				auto ec, auto&&...) mutable {
				if (ec) throw boost::system::system_error{ec};
				p.set_value(std::move(res));
			});
	});
}

inline auto write(
	ws::stream<tcp::socket&>& socket, std::string_view msg)
{
	return cti::make_continuable<void>(
		[&, msg = static_cast<std::string>(msg)](auto&& p) mutable {
			auto b = std::make_shared<std::string>(std::move(msg));
			socket.async_write(ba::buffer(*b),
				[&, b = std::move(b),
					p = std::forward<decltype(p)>(p)](
					auto ec, auto&&...) mutable {
					if (ec) throw boost::system::system_error{ec};
					p.set_value();
				});
		});
}

template <typename T>
auto read(ws::stream<tcp::socket&>& socket, T& buffer)
{
	return cti::make_continuable<void>([&](auto&& p) {
		socket.async_read(buffer,
			[&, p = std::forward<decltype(p)>(p)](
				auto ec, auto&&...) mutable {
				if (ec) throw boost::system::system_error{ec};
				p.set_value();
			});
	});
}

template <typename T>
auto write(
	tcp::socket& socket, std::shared_ptr<http::request<T>>&& req)
{
	return cti::make_continuable<void>(
		[&, req = std::move(req)](auto&& p) mutable {
			http::async_write(socket, *req,
				[&, req = std::move(req),
					p = std::forward<decltype(p)>(p)](
					auto ec, auto&&...) mutable {
					if (ec) throw boost::system::system_error{ec};
					p.set_value();
				});
		});
}

inline auto resolve(ba::io_service& io, tcp::resolver::query&& q)
{
	return cti::make_continuable<tcp::endpoint>(
		[&, q = std::move(q)](auto&& p) {
			auto r = std::make_shared<tcp::resolver>(io);
			r->async_resolve(q,
				[&, p = std::forward<decltype(p)>(p),
					r = std::move(r)](auto ec, auto it) mutable {
					if (ec) throw boost::system::system_error{ec};
					p.set_value(it->endpoint());
				});
		});
}
} // namespace chrome_remote::impl

// vim: ts=4:sts=0:noet:sw=0
