#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <iostream>
namespace net = boost::asio;
namespace beast = boost::beast;
using namespace boost::beast;
using namespace boost::beast::websocket;

int main(int argc, char const *argv[]) {

  try
  {
    net::io_context ioc;
    std::string host = "localhost";
    stream<net::ip::tcp::socket> ws(ioc);
    net::ip::tcp::resolver resolver(ioc);
    auto const resolved = resolver.resolve(host, "80");
    // Connect the socket to the IP address returned from performing a name lookup
    net::connect(ws.next_layer(), resolved.begin(), resolved.end());

    ws.handshake(host, "/");
    std::cout << "Connected." << '\n';
    std::string msg;
    ws.text(true);
    while(msg != "!quit~")
    {
      std::cin >> msg;
      ws.write(net::buffer(msg));
    }
    ws.close(close_code::normal);
  }
  catch(error_code ec)
  {
    if(ec == websocket::error::closed)
    {
      return 0;
    }
  }
  return 0;
}
