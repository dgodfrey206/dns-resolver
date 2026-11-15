#include <asio.hpp>
#include <cstdlib>
#include <iostream>
#include <cstdint>

using asio::ip::tcp;
using asio::ip::udp;

void send_packet(udp::socket& socket, udp::endpoint& endpoint) {
	std::vector<std::uint8_t> buf = {
		0x14, 0x1F, // Transaction ID (5151)
		0x01, 0x0, // Flags (Set: Recursion Desired)
		0x0, 0x01, // Questions count
		0x0, 0x0, // Answer RRs
		0x0, 0x0, // Authority RRs
		0x0, 0x0, // Additional RRs
		0x6, 'g', 'o', 'o', 'g', 'l', 'e',
		0x3, 'c', 'o', 'm', 0x0,
		0x0, 0x01, // Question Type (A Record)
		0x0, 0x01 // Question Class
	};
	asio::const_buffer buffer = asio::buffer(buf);
	auto len = socket.send_to(buffer, endpoint);

	if (len) {
		std::cout << "Successfully sent " << len << " bytes\n";
	} else {
		std::cerr << "Failed to send packet\n";
	}
}

int main(int argc, char* argv[]) {
  try {
	asio::io_context io_context;

	udp::endpoint endpoint(asio::ip::address::from_string("127.0.0.1"), 8080);
	udp::socket socket(io_context);
	socket.open(asio::ip::udp::v4());

	send_packet(socket, endpoint);
  } catch (std::exception& e) {
	std::cerr << "Exception: " << e.what();
  }
}
