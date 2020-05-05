#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <pthread.h>
#include <vector>
namespace net = boost::asio;
namespace beast = boost::beast;
using namespace boost::beast;
using namespace boost::beast::websocket;

int main(int argc, char const *argv[]) {
  std::vector<pthread_t> clients;
  net::io_context ioc;
  net::ip::tcp::acceptor acceptor(ioc);
  net::ip::tcp::endpoint endpoint(net::ip::tcp::v4(), 80);
  acceptor.open(endpoint.protocol());
  acceptor.bind(endpoint);
  std::cout << "Endpoint Bound." << '\n';
  acceptor.listen();
  std::cout << "Listening..." << '\n';
  // The socket returned by accept() will be forwarded to the tcp_stream,
  // which uses it to perform a move-construction from the net::ip::tcp::socket.

  stream<net::ip::tcp::socket> ws(acceptor.accept());
  // Perform the websocket handshake in the server role.
  // The stream must already be connected to the peer.

  while(true)
  {
    ws.accept();
    std::cout << "Handshake complete." << '\n';
    clients.push_back();
    pthread_create(&clients.back(),NULL,&clientHandler,NULL);
  }
  return 0;
}

void* clientHandler (void*)
{
  while(true)
  {
    multi_buffer buffer;
    ws.read(buffer);
    std::string msg = buffers_to_string(buffer.data());
    std::cout << msg << '\n';
  }
}
