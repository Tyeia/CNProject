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
        std::string msg;
        multi_buffer reset;
        buffer=reset;
        ws.read(buffer);
        ws.text(ws.got_text());
        msg = buffers_to_string(buffer.data());
        while(!ws.is_message_done())
        {
          std::cout << msg << ' ';
          ws.read(buffer);
          msg = buffers_to_string(buffer.data());
        }
        std::cout << msg << "\n";
      }
    }
  }
  catch(error_code ec)
  {
    if(ec == net::error::operation_aborted||ec==websocket::error::closed)
    {
      return 0;
    }
  }
  return 0;
}
