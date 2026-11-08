#include <asio.hpp>
#include <cstdlib>
#include <iostream>
#include <cstdint>

using asio::ip::tcp;
using asio::ip::udp;

#pragma pack(push, 1)
struct packet {
	// 12 byte header
        uint16_t trans_id = 5151;
        uint16_t flags = 1 << 15;
	uint16_t question_count = 1;
	uint16_t answer_count = 0;
	uint16_t authority_count = 0;
	uint16_t additional_count = 0;
	// ^ end of header
	// Question entries
	char question_name[20] = "6google3com0";
	int questionType = 0x01; // Return A record
	int question_class = 0x0001;
};
#pragma pack(pop)

void send_packet(udp::socket& socket, udp::endpoint& endpoint) {
	packet pkt;
	asio::const_buffer buffer = asio::buffer(&pkt, sizeof(pkt));
	auto len = socket.send_to(buffer, endpoint);

	if (len) {
		std::cout << "Successful\n";
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
