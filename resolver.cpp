#include <asio.hpp>
#include <iostream>
#include <string>
#include <ctime>

using asio::ip::tcp;

std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

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
      std::cout.write(&message[0], 12);
      std::cout << '\n';
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
