#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <cppcodec/base64_default_rfc4648.hpp>
#include <deque>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <json.hpp>
#include <regex>
#include <string_view>

#include "asio_continuable.h"

namespace chrome_remote {
using json = nlohmann::json;
namespace ba = boost::asio;
using tcp = ba::ip::tcp;
namespace be = boost::beast;
namespace ws = be::websocket;
namespace http = be::http;

namespace impl {
	std::string ws_addr(const std::string& str);
	void save(std::string_view filename,
		const std::vector<std::uint8_t>& data);
} // namespace impl

class ChromeRemote {
private:
	auto resolve_host(const std::string& host_)
	{
		static const std::regex rp{R"_(\s*([^:]+):([^\s]+)\s*)_"};
		std::smatch parts;
		std::regex_search(host_, parts, rp);
		host = fmt::format("{}:{}", parts[1], parts[2]);
		return impl::resolve(
			socket.get_io_service(), {parts[1], parts[2]})
			.then([&](auto&& ep) {
				endpoint = std::forward<decltype(ep)>(ep);
			});
	}

	auto connect()
	{
		using Request = http::request<http::string_body>;
		using Response = http::response<http::string_body>;
		return impl::connect(socket, endpoint)
			.then([&] {
				auto req = std::make_shared<Request>(
					http::verb::get, "/json", 11);
				req->set(http::field::host, host);
				return impl::write(socket, std::move(req));
			})
			.then(impl::read<Response>(socket, buffer))
			.then([&](auto&& res) {
				return impl::handshake(
					websocket, host, impl::ws_addr(res->body()));
			});
	}

	auto send(const std::string& msg)
	{
		return impl::write(websocket, msg);
	}

	auto receive()
	{
		return impl::read(websocket, buffer).then([&] {
			const auto ptr{
				ba::buffer_cast<const char*>(buffer.data())};
			auto r{json::parse(
				ptr, ptr + ba::buffer_size(buffer.data()))};
			buffer.consume(buffer.size());
			return r;
		});
	}

	template <typename T> void poll(T&& p, int this_id)
	{
		receive().then(
			[=, p = std::forward<T>(p)](auto&& val) mutable {
				if (auto it{val.find("id")}; it != val.end()
					&& static_cast<int>(*it) == this_id) {
					p.set_value(std::forward<decltype(val)>(val));
					return;
				}
				log.emplace_front(std::forward<decltype(val)>(val));
				poll(std::move(p), this_id);
			});
	}

	template <typename F, typename T> void poll_wait(F f, T&& p)
	{
		receive().then(
			[=, p = std::forward<T>(p)](auto&& val) mutable {
				if (f(val)) {
					p.set_value(std::forward<decltype(val)>(val));
					return;
				}
				poll_wait(std::move(f), std::move(p));
			});
	}

public:
	ChromeRemote(ba::io_service& io);

	auto call(const std::string& method, json params = json({}))
	{
		const auto this_id{message_id++};
		json m{{"id", this_id}, {"method", method},
			{"params", std::move(params)}};
		return send(m.dump()).then(
			cti::make_continuable<json>([&, this_id](auto&& p) {
				poll(std::forward<decltype(p)>(p), this_id);
			}));
	}

	auto poll()
	{
		return cti::make_continuable<json>([&](auto&& p) {
			if (log.empty()) {
				receive().then([p = std::forward<decltype(p)>(p)](
								   auto&& val) mutable {
					p.set_value(std::forward<decltype(val)>(val));
				});
				return;
			}
			p.set_value(std::move(log.back()));
			log.pop_back();
		});
	}

	template <typename F> auto poll(F f)
	{
		return cti::make_continuable<json>([&, f](auto&& p) mutable {
			if (log.empty()) {
				poll_wait(std::move(f), std::forward<decltype(p)>(p));
				return;
			}
			json val{std::move(log.back())};
			log.pop_back();
			if (f(val)) {
				p.set_value(std::move(val));
				return;
			}
			poll_wait(std::move(f), std::forward<decltype(p)>(p));
		});
	}

	auto connect(const std::string& host_)
	{
		return resolve_host(host_).then(connect());
	}

private:
	tcp::socket socket;
	tcp::endpoint endpoint;
	be::flat_buffer buffer;
	ws::stream<tcp::socket&> websocket{socket};
	int message_id{0};
	std::deque<json> log;
	std::string host;
};

inline auto print_to_pdf(ChromeRemote& remote, std::string_view url,
	std::string_view filename)
{
	return remote.call("Page.enable")
		.then(remote.call("Page.navigate",
			{{"url", static_cast<std::string>(url)}}))
		.then(remote.poll([](const auto& val) {
			return static_cast<const std::string&>(val["method"])
				== "Page.frameStoppedLoading";
		}))
		.then(remote.call("Page.printToPDF",
			{// clang-format off
				{"landscape", false},
				{"displayHeaderFooter", false},
				{"printBackground", false},
				{"paperWidth", 8.27f},
				{"paperHeight", 11.69f},
				{"marginTop", 0},
				{"marginLeft", 0},
				{"marginRight", 0},
				{"marginBottom", 0},
				{"pageRanges", "1"}}))
		// clang-format on
		.then([&, filename](auto&& val) {
			const std::vector<std::uint8_t> data{
				base64::decode(static_cast<const std::string&>(
					val["result"]["data"]))};
			impl::save(filename, data);
		});
}

} // namespace chrome_remote

// vim: ts=4:sts=0:noet:sw=0
