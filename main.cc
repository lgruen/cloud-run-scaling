#include <chrono>
#include <cstdlib>
#include <ctime>
#include <httpserver.hpp>
#include <iostream>
#include <thread>

using namespace httpserver;

class HttpHandler : public httpserver::http_resource {
 public:
  const std::shared_ptr<httpserver::http_response> render(
      const httpserver::http_request &) {
    const std::chrono::system_clock::time_point received_time =
        std::chrono::system_clock::now();
    const std::time_t received_time_t =
        std::chrono::system_clock::to_time_t(received_time);
    // Pretend to do some work.
    std::this_thread::sleep_for(std::chrono::seconds(10));
    // The response consists of the time that we received the request.
    return std::make_shared<httpserver::string_response>(
        std::string("Received at: ") + std::ctime(&received_time_t));
  }
};

int main(int, char **) {
  const char *const port_env = std::getenv("PORT");
  if (port_env == nullptr) {
    std::cerr << "Missing PORT environment variable" << std::endl;
    return 1;
  }

  const int port = std::atoi(port_env);
  if (!port) {
    std::cerr << "Failed to parse PORT environment variable" << std::endl;
    return 1;
  }

  std::cout << "Starting server on port " << port << "..." << std::endl;

  httpserver::webserver webserver = httpserver::create_webserver(port);

  HttpHandler http_handler;
  webserver.register_resource("/received_time", &http_handler);
  webserver.start(true);

  return 0;
}