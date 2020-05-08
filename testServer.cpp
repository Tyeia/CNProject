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
    net::ip::tcp::acceptor acceptor1(ioc);
    net::ip::tcp::acceptor acceptor2(ioc);
    net::ip::tcp::endpoint endpoint(net::ip::tcp::v4(), 80);
    acceptor1.open(endpoint.protocol());
    acceptor2.open(endpoint.protocol());
    acceptor1.set_option(net::socket_base::reuse_address(true));
    acceptor2.set_option(net::socket_base::reuse_address(true));
    acceptor1.bind(endpoint);
    acceptor2.bind(endpoint);
    std::cout << "Endpoint Bound." << '\n';
    while(true)
    {
      acceptor1.listen();
      std::cout << "Listening..." << '\n';
      stream<net::ssl::stream<net::ip::tcp::socket>> ws1(acceptor1.accept(),ctx);
      ws1.next_layer().handshake(net::ssl::stream_base::server);
      ws1.accept();
      std::cout << "Handshake complete." << '\n';
      acceptor2.listen();
      std::cout << "Listening..." << '\n';
      // The socket returned by accept() will be forwarded to the tcp_stream,
      // which uses it to perform a move-construction from the net::ip::tcp::socket.

      stream<net::ssl::stream<net::ip::tcp::socket>> ws2(acceptor2.accept(),ctx);
      // Perform the websocket handshake in the server role.
      // The stream must already be connected to the peer.
      ws2.next_layer().handshake(net::ssl::stream_base::server);
      ws2.accept();
      std::cout << "Handshake complete." << '\n';
      multi_buffer buffer;
      while(buffers_to_string(buffer.data()).find("!quit~") == std::string::npos)
      {
        std::string msg1, msg2;
        multi_buffer reset;
        buffer=reset;

        ws1.read(buffer);
        ws1.text(ws1.got_text());
        msg1 = buffers_to_string(buffer.data());
        if(msg1.find("!quit~") == std::string::npos)
        {
          std::cout << msg1 << "\n";
          ws2.write(net::buffer(msg1));
        }
        ws2.read(buffer);
        msg2 = buffers_to_string(buffer.data());
        if(msg2.find("!quit~") == std::string::npos)
        {
          std::cout << msg2 << '\n';
          ws1.write(net::buffer(msg2));
        }
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
