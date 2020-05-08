#include <boost/beast.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
namespace net = boost::asio;
namespace beast = boost::beast;
using namespace boost::beast;
using namespace boost::beast::websocket;

int main(int argc, char const *argv[]) {
  try
  {
    net::io_context ioc;
    net::ssl::context ctx(net::ssl::context::tlsv12);
    ctx.use_certificate_file("CNProjectNewCert.pem", net::ssl::context::pem);
    ctx.use_rsa_private_key_file("CNProject.pem",net::ssl::context::pem);
    std::make_shared<net::ip::tcp::acceptor> acceptor(ioc);
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

      stream<net::ssl::stream<net::ip::tcp::socket>> ws(acceptor.accept(),ctx);
      // Perform the websocket handshake in the server role.
      // The stream must already be connected to the peer.
      ws.next_layer().handshake(net::ssl::stream_base::server);
      ws.accept();
      std::cout << "Handshake complete." << '\n';
      multi_buffer buffer;
      while(buffers_to_string(buffer.data()).find("!quit~") == std::string::npos)
      {
        std::string msg;
        std::stringstream msg_buffer;
        multi_buffer reset;
        buffer=reset;
        ws.read(buffer);
        ws.text(ws.got_text());
        msg_buffer << buffers_to_string(buffer.data());
        msg = msg_buffer.str();
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
    else
    {
      int nothing;
      std::cout << ec << '\n';
      std::cin >> nothing;
    }
  }
  return 0;
}
