#include <boost/beast.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <stack>
namespace net = boost::asio;
namespace beast = boost::beast;
using namespace boost::beast;
using namespace boost::beast::websocket;

int clientNum;
std::stack<int> messageStack;
int activeClients = 0;

void* socketServer(void*)
{
  try
  {
    net::io_context ioc;
    net::ssl::context ctx(net::ssl::context::tlsv12);
    ctx.use_certificate_file("CNProjectNewCert.pem", net::ssl::context::pem);
    ctx.use_rsa_private_key_file("CNProject.pem",net::ssl::context::pem);
    net::ip::tcp::acceptor acceptor(ioc);
    net::ip::tcp::endpoint endpoint(net::ip::tcp::v4(), 80);
    acceptor.open(endpoint.protocol());
    acceptor.bind(endpoint);
    std::cout << "Endpoint Bound." << '\n';
    while(true)
    {
      acceptor.listen(net::socket_base::max_listen_connections);
      std::cout << "Listening..." << '\n';
      // The socket returned by accept() will be forwarded to the tcp_stream,
      // which uses it to perform a move-construction from the net::ip::tcp::socket.
      acceptor.set_option(net::socket_base::reuse_address(true));
      stream<net::ssl::stream<net::ip::tcp::socket>> ws(acceptor.accept(),ctx);
      // Perform the websocket handshake in the server role.
      // The stream must already be connected to the peer.
      ws.next_layer().handshake(net::ssl::stream_base::server);
      ws.accept();
      activeClients++;
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
        if(msg != "!quit~")
        {
          std::cout << msg << "\n";
          for(int i = 0; i<activeClients; i++)
          {
            messageStack.push(i);
          }
          if(messageStack.size()==activeClients)
          {
            messageStack.pop();
            while(!messageStack.empty());
          }
          else
          {
            ws.write(net::buffer(msg));
            messageStack.pop();
            while(!messageStack.empty());
          }
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
      std::cout << ec.message() << '\n';
      std::cin >> nothing;
    }
  }
  activeClients--;
}

int main(int argc, char const *argv[]) {
  std::cout << "How many clients?: ";
  std::cin >> clientNum;
  pthread_t clients[clientNum];
  net::io_context ioc{clientNum}
  for(int i = 0; i<clientNum;i++)
  {
    ioc.run();
    pthread_create(&clients[i],NULL,&socketServer,NULL);
    pthread_join(clients[i],NULL);
  }
  return 0;
}
