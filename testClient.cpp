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
    net::io_context ioc; //i_o context req for all I/O
    net::ssl::context ctx(net::ssl::context::tlsv12); //context for SSL security
    
    //Set up for SSL
    ctx.set_default_verify_paths();
    ctx.use_certificate_file("CNProjectNewCert.pem", net::ssl::context::pem); //Generated Cert
    ctx.use_rsa_private_key_file("CNProject.pem",net::ssl::context::pem); //Generated Key
    
    //Set up network connection
    std::string host = "71.213.21.10"; //IP address for client to connect to
    stream<net::ssl::stream<net::ip::tcp::socket>> ws(ioc, ctx);
    net::ip::tcp::resolver resolver(ioc);
    auto const resolved = resolver.resolve(host, "80"); //connect through port 80
    // Connect the socket to the IP address returned from performing a name lookup
    net::connect(ws.lowest_layer(), resolved.begin(), resolved.end());

    //Get user name to display with messages
    std::string name;
    std::cout << "State your name: ";
    std::cin >> name;

    bool flag = false;
    ws.next_layer().handshake(net::ssl::stream_base::client);
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
    else
    {
      int nothing;
      std::cout << ec << '\n';
      std::cin >> nothing;
    }
  }
  return 0;
}
