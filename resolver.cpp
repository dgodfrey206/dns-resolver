#include <asio.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <bits/stdc++.h>
using namespace std;
using asio::ip::tcp;

uint16_t generate_transaction_id() {
    std::random_device rd{};
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint16_t> distrib(0, 65535);
    return distrib(gen);
}

std::string truncate_qname(std::vector<std::string> const& domain_parts) {
	uint8_t i = domain_parts.back().size();
	return std::to_string(i) + domain_parts.back();
}

#pragma pack(push, 1)
struct packet {
        uint16_t trans_id;
        uint16_t flags;
        uint16_t question_count;
        uint16_t answer_count;
        uint16_t authority_count;
        uint16_t additional_count;
};
#pragma pack(pop)

class dns_resolver {
public:
    dns_resolver(asio::io_context& io_context, uint16_t port)
        : socket_(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port))
    {
        start_receive();
    }

private:
    void start_receive() {
        // Allocate a fresh buffer for the incoming message
        // 512 bytes is standard for basic UDP DNS, or 4096 for EDNS0
        auto buffer = std::make_shared<std::vector<uint8_t>>(4096);
        auto remote_endpoint = std::make_shared<asio::ip::udp::endpoint>();

        // Register the async read operation
        socket_.async_receive_from(
            asio::buffer(*buffer), *remote_endpoint,
            [this, buffer, remote_endpoint](const asio::error_code& error, size_t bytes_transferred) {
                if (socket_.is_open()) {
                    start_receive();
                }

                if (!error && bytes_transferred > 0) {
                    handle_packet(buffer, bytes_transferred, *remote_endpoint);
                } else {
                    std::cerr << "Receive error: " << error.message() << "\n";
                }
            });
    }

    void handle_packet(std::shared_ptr<std::vector<uint8_t>> buffer,
                       size_t length,
                       const asio::ip::udp::endpoint& client_endpoint) {
        std::cout << "Received " << length << " bytes from " << client_endpoint << "\n";

        // Your DNS parsing/forwarding logic goes here...
    }

    uint16_t generate_transaction_id() {
        std::random_device rd{};
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint16_t> distrib(0, 65535);
        return distrib(gen);
    }

    std::string truncate_qname(std::vector<std::string> const& domain_parts) {
        uint8_t i = domain_parts.back().size();
        return std::to_string(i) + domain_parts.back();
    }

    asio::ip::udp::socket socket_;
};

int main()
{
  using namespace std;
  try
  {
    asio::io_context io_context;
    dns_resolver dns(io_context, 8080);
    /*
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
    packet response{};
    //for (;;)
    {
      tcp::socket socket(io_context);
      acceptor.accept(socket);

      unsigned char header[12]{};
      std::error_code ignored_error;
      size_t len = socket.read_some(asio::buffer(header), ignored_error);

      response.trans_id = generate_transaction_id();

      uint16_t question_count;
      memcpy(&question_count, header + 4, sizeof(uint16_t));

      if (question_count != 0) {
	// Has question entries
	std::vector<std::string> domain_parts;
	uint8_t next_byte;
        while (socket.read_some(asio::buffer(&next_byte, 1), ignored_error) && next_byte != 0) {
		int token_length = static_cast<int>(next_byte) - '0';
		std::string str;
		str.resize(token_length);
		socket.read_some(asio::buffer(str), ignored_error);
		domain_parts.push_back(str);
	}

	// read the rest of the packet
	uint16_t question_type, question_class;
	socket.read_some(asio::buffer(&question_type, 1), ignored_error);
	socket.read_some(asio::buffer(&question_class, 1), ignored_error);

	tcp::socket messenger(io_context);
	tcp::endpoint endpoint(asio::ip::address::from_string("192.33.4.12"), 53);
	messenger.connect(endpoint);

	auto truncated = truncate_qname(domain_parts);
	asio::write(messenger, asio::buffer(&response, sizeof(packet)), ignored_error);
	asio::write(messenger, asio::buffer(truncated), ignored_error);
	asio::write(messenger, asio::buffer(&question_type, sizeof question_type), ignored_error);
	asio::write(messenger, asio::buffer(&question_class, sizeof question_class), ignored_error);
      }
    }*/
    io_context.run();
  }
  catch (std::exception& e) {
    std::cerr << "Exception message: " << e.what() << std::endl;
  }

  return 0;
}
