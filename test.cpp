#include <asio.hpp>
#include <cstdlib>
#include <iostream>
#include <cstdint>

using asio::ip::tcp;

#pragma pack(push, 1)
struct packet {
        uint16_t trans_id = 0x345d;
        uint16_t flags = 1 << 9;
        uint16_t num_q = 1;
        uint16_t num_ansrr = 0;
        uint16_t num_authrr = 0;
	uint16_t num_addrr = 0;
	char name[28] = "0x03www0x06google0x03com0x0";
	int questionType = 0x01;
	int questionClass = 0x0001;
};
#pragma pack(pop)

void send_packet(tcp::socket& socket) {
	packet pkt;
	asio::const_buffer buffer = asio::buffer(&pkt, sizeof(pkt));
	asio::error_code ec;
	asio::write(socket, buffer, ec);

	if (!ec) {
		std::cout << "Successful";
	} else {
		std::cerr << "Failed to send packet: " << ec.message() << '\n';
	}
}

int main(int argc, char* argv[]) {
  try {
	asio::io_context io_context;

	tcp::endpoint endpoint(asio::ip::address::from_string("127.0.0.1"), 8080);
	tcp::socket socket(io_context);
	socket.connect(endpoint);

	send_packet(socket);
  } catch (std::exception& e) {
	std::cerr << "Exception: " << e.what();
  }
}
