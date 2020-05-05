#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <iostream>
namespace net = boost::asio;
namespace beast = boost::beast;
using namespace boost::beast;
using namespace boost::beast::websocket;

int main(int argc, char const *argv[]) {
  net::io_context ioc;
  stream<net::ip::tcp::socket> ws(ioc);
  net::ip::tcp::resolver resolver(ioc);
  auto const resolved = resolver.resolve("192.168.0.222", "80");
  // Connect the socket to the IP address returned from performing a name lookup
  net::connect(ws.next_layer(), resolved.begin(), resolved.end());

  ws.handshake("192.168.0.100", "/");
  std::cout << "Connected." << '\n';
  while(true)
  {
    std::string msg;
    std::cin >> msg;
    ws.write(net::buffer(msg));
  }
  return 0;
}
