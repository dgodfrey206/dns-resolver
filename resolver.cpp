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

int main()
{
  using namespace std;
  try
  {
    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
    //for (;;)
    {
      tcp::socket socket(io_context);
      acceptor.accept(socket);

      unsigned char header[12]{};
      std::error_code ignored_error;
      size_t len = socket.read_some(asio::buffer(header), ignored_error);
      std::cout << len << '\n';
      // unset RD flag
      *(header + 2) |= (1 << 7);

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

	
      }
    }
  }
  catch (std::exception& e) {
    std::cerr << "Exception message: " << e.what() << std::endl;
  }

  return 0;
}
