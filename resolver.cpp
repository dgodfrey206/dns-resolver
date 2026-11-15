#include <asio.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <bits/stdc++.h>
using namespace std;
using asio::ip::tcp;

enum class LookupState {
    AWAITING_ROOT_RESPONSE,
    AWAITING_TLD_RESPONSE,
	AWAITING_AUTH_RESPONSE
};

using Buffer = std::vector<std::uint8_t>;

inline constexpr auto HEADER_BLOCK_SIZE = 12;

class dns_resolver {
public:
    dns_resolver(asio::io_context& io_context, uint16_t port)
        : socket_(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port))
    {
        start_receive();
    }

private:
    void start_receive() {
        auto buffer = std::make_shared<std::vector<uint8_t>>(4096);
        auto remote_endpoint = std::make_shared<asio::ip::udp::endpoint>();

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

        uint16_t trans_id = ((*buffer)[0] << 7) | (*buffer)[1];

        if (record.find(trans_id) == record.end()) {
            std::vector<std::uint8_t> top_level_domain;
            int start = 12, i = start, prev;
            for (; i < buffer->size() && (*buffer)[i] != 0; i++) {
                prev = i;
                i += (*buffer)[i];
            }

            if (!(i < buffer->size())) {
                // something went wrong
                std::cerr << "Something went wrong\n";
                return;
            }

            record[trans_id].status = { LookupState::AWAITING_ROOT_RESPONSE, {buffer->data() + start, buffer->data() + i} };
            top_level_domain.assign(buffer->data() + prev + 1, buffer->data() + prev + 1 + (*buffer)[prev]);

            // send packet to root nameserver
    /*
            std::vector<std::uint8_t> response_buf = {
                        tid(), tid(),
                        0x0, 0x0,
                        0x0, 0x0,
                        0x0, 0x0,
                        0x0, 0x0,
                        0x0, 0x0,
                static_cast<std::uint8_t>(str.size())
                };

            for (int i = 0; i < str.size(); i++) response_buf.push_back(str[i]);
            for (int x : {0x0, 0x01, 0x0, 0x01}) response_buf.push_back(x);*/
	    }

    }

    Buffer::const_iterator skip_record(Buffer::const_iterator it, int type) {
        /*
          type == 1 header
          type == 2 question
          type == 3 resource record
        */
        if (type == 1) {
            return it + HEADER_BLOCK_SIZE; // skip header
        }
        if (type == 2) {
            for (; it != Buffer::const_iterator{} && *it != 0; it++) {
                it += *it;
            }
            return it;
        }

    }

    uint8_t tid() {
        std::random_device rd{};
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint8_t> distrib(0, 255);
        return distrib(gen);
    }

	struct request_state {
		LookupState status;
		std::vector<std::uint8_t> full_domain;
	};

	asio::ip::udp::socket socket_;
	std::unordered_map<int16_t, request_state> record;
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
    std::vector<std::uint8_t> response;
    //for (;;)
    {
      tcp::socket socket(io_context);
      acceptor.accept(socket);

      unsigned char header[12]{};
      std::error_code ignored_error;
      size_t len = socket.read_some(asio::buffer(header), ignored_error);

      uint16_t trans_id = generate_transaction_id();
      response.push_back(trans_id >> 8);
      response.push_back(trans_id);

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
