#include <asio.hpp>
#include <iostream>
#include <string>
#include <ctime>

using asio::ip::tcp;

int main()
{
  try
  {
    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
    //for (;;)
    {
      tcp::socket socket(io_context);
      acceptor.accept(socket);

      std::array<char, 12> message{};
      std::error_code ignored_error;
      size_t len = socket.read_some(asio::buffer(message), ignored_error);
      std::cout << len << '\n';
      std::string id(&message[0], &message[0] + 2);
      std::cout << id;
      std::cout << '\n';
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
