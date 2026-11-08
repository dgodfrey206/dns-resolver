#include <asio.hpp>
#include <cstdlib>
#include <iostream>
#include <cstdint>

using asio::ip::tcp;

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

void send_packet(tcp::socket& socket) {
	packet pkt;
	asio::const_buffer buffer = asio::buffer(&pkt, sizeof(pkt));
	asio::error_code ec;
	asio::write(socket, buffer, ec);

	if (!ec) {
		std::cout << "Successful\n";
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
