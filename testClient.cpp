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

    std::string name;
    std::cout << "State your name: ";
    std::cin >> name;
    bool flag = false;
    ws.handshake(host, "/");
    std::cout << "Connected." << '\n';
    std::string msg;
    ws.text(true);
    while(msg != "!quit~")
    {
      if(flag)
      {
        std::cout << "[" << name << "]: ";
      }
      std::getline(std::cin,msg);
      if(flag)
      {
        ws.write(net::buffer("["+name+"]: "+msg));
      }
      flag=true;
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
