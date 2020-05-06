#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <iostream>
namespace net = boost::asio;
namespace beast = boost::beast;
using namespace boost::beast;
using namespace boost::beast::websocket;

int main(int argc, char const *argv[]) {
  net::io_context ioc;
  net::ip::tcp::acceptor acceptor(ioc);
  net::ip::tcp::endpoint endpoint(net::ip::tcp::v4(), 80);
  acceptor.open(endpoint.protocol());
  acceptor.bind(endpoint);
  std::cout << "Endpoint Bound." << '\n';
  while(true)
  {
    acceptor.listen();
    std::cout << "Listening..." << '\n';
    // The socket returned by accept() will be forwarded to the tcp_stream,
    // which uses it to perform a move-construction from the net::ip::tcp::socket.

    stream<net::ip::tcp::socket> ws(acceptor.accept());
    // Perform the websocket handshake in the server role.
    // The stream must already be connected to the peer.

    ws.accept();
    std::cout << "Handshake complete." << '\n';
    multi_buffer buffer;
    while(buffers_to_string(buffer.data())!= "!quit~")
    {

      char compare = '0';
      std::string msg;

      while(compare != '\n')
      {
        multi_buffer reset;
        buffer=reset;
        compare = buffers_to_string(buffer.data()).back();
        ws.read(buffer);
        ws.text(ws.got_text());
        msg = buffers_to_string(buffer.data());
        msg.erase(std::remove(msg.begin(), msg.end(), '\n'), msg.end());
        std::cout << msg << ' ';
      }
      std::cout << msg<< '\n';
    }
  }
  return 0;
}
