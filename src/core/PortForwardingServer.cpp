/*
 * @name BookFiler Module - HTTP
 * @author Branden Lee
 * @version 1.02
 * @license MIT
 * @brief HTTP module for BookFiler™ applications.
 */

// Local Project
#include "PortForwarding.hpp"

/*
 * bookfiler - port
 * Port fowarding utilities. Initially this utility was made to forward mysql
 * traffic through a secure ssh connection.
 */
namespace bookfiler {
namespace port {

ForwardingServerImpl::ForwardingServerImpl(
    std::map<std::string, bookfiler::port::newForwardingServerType> map_) {
  for (auto val : map_) {
    if (int *val_ = std::get_if<int>(&val.second)) {
      if (val.first == "sourcePort") {
        serverState->sourcePort = static_cast<unsigned short>(*val_);
      } else if (val.first == "destPort") {
        serverState->destPort = static_cast<unsigned short>(*val_);
      }
    } else if (double *val_ = std::get_if<double>(&val.second)) {
    } else if (std::string *val_ = std::get_if<std::string>(&val.second)) {
      if (val.first == "sourceAddress") {
        serverState->sourceAddress = boost::asio::ip::make_address(*val_);
      } else if (val.first == "destAddress") {
        serverState->destAddress = boost::asio::ip::make_address(*val_);
      }
    }
  }
}

ForwardingServerImpl::~ForwardingServerImpl() {
  for (auto &thread : threadList) {
    thread.join();
  }
}

int ForwardingServerImpl::setSettingsDoc(
    std::shared_ptr<rapidjson::Value> settingsDoc_) {
  settingsDoc = settingsDoc_;
  return 0;
}

int ForwardingServerImpl::run() {

  // dst_socket(socket.get_io_service());
  // src_socket(std::move(socket));

  std::stringstream ss;
  auto hardwareThreadsNum = std::thread::hardware_concurrency();
  ss << "threadsNum: " << serverState->threadsNum
     << "\nHardware Threads: " << hardwareThreadsNum;
  serverState->threadsNum = hardwareThreadsNum;
  logStatus("::ForwardingServerImpl::runAsync", ss.str());
  // hint how much concurrency is used for the io context
  ioContext =
      std::make_shared<boost::asio::io_context>(serverState->threadsNum);
  // Create and launch a listening port
  net::spawn(
      *ioContext,
      std::bind(&Listener::run,
                std::make_shared<Listener>(
                    std::ref(*ioContext),
                    tcp::endpoint{serverState->sourceAddress, serverState->sourcePort},
                    serverState),
                std::placeholders::_1));

  // Run the I/O service on the requested number of threads
  threadList.reserve(serverState->threadsNum);
  for (auto i = serverState->threadsNum; i > 0; --i) {
    threadList.emplace_back(&ForwardingServerImpl::runIoContext, this);
  }

  return 0;
}

void ForwardingServerImpl::runIoContext() { ioContext->run(); }

void ForwardingServerImpl::dst_connect_handler(boost::system::error_code ec) {
  if (ec) {
    src_socket.close();
    dst_socket.close();
    return;
  }
  src_socket.async_read_some(
      boost::asio::buffer(src_read_buffer, read_buffer_length),
      BIND_HANDLER_S(src_read_some_handler));
  dst_socket.async_read_some(
      boost::asio::buffer(dst_read_buffer, read_buffer_length),
      BIND_HANDLER_S(dst_read_some_handler));
}

void ForwardingServerImpl::src_read_some_handler(boost::system::error_code ec,
                                                 std::size_t length) {
  if (ec) {
    src_socket.close();
    dst_socket.close();
    return;
  }
  enqueue_dst_write(length);
  src_socket.async_read_some(
      boost::asio::buffer(src_read_buffer, read_buffer_length),
      BIND_HANDLER_S(src_read_some_handler));
}

void ForwardingServerImpl::dst_read_some_handler(boost::system::error_code ec,
                                                 std::size_t length) {
  if (ec) {
    src_socket.close();
    dst_socket.close();
    return;
  }
  enqueue_src_write(length);
  dst_socket.async_read_some(
      boost::asio::buffer(dst_read_buffer, read_buffer_length),
      BIND_HANDLER_S(dst_read_some_handler));
}

void ForwardingServerImpl::src_write_handler(boost::system::error_code ec, std::size_t) {
  write_buffer_type buffer = src_write_queue.front();
  src_write_queue.pop();
  std::cout << "] deallocating buffer 0x" << buffer.first << "["
            << buffer.second << "]" << std::endl;
  boost::asio::asio_handler_deallocate(buffer.first, buffer.second);
  if (ec) {
    src_socket.close();
    dst_socket.close();
    return;
  }
}

void ForwardingServerImpl::dst_write_handler(boost::system::error_code ec, std::size_t) {
  write_buffer_type buffer = dst_write_queue.front();
  dst_write_queue.pop();
  std::cout << "] deallocating buffer 0x" << buffer.first << "["
            << buffer.second << "]" << std::endl;
  boost::asio::asio_handler_deallocate(buffer.first, buffer.second);
  if (ec) {
    src_socket.close();
    dst_socket.close();
    return;
  }
}

void ForwardingServerImpl::enqueue_src_write(std::size_t length) {
  void *buffer_data = boost::asio::asio_handler_allocate(
      length, &BIND_HANDLER_S(src_write_handler));
  std::cout << "[ allocated buffer 0x" << buffer_data << "[" << length << "]"
            << std::endl;
  // std::copy(dst_read_buffer, dst_read_buffer + length,
  // reinterpret_cast<char*>(buffer_data));
  std::memcpy(buffer_data, dst_read_buffer, length);
  write_buffer_type buffer(buffer_data, length);
  src_write_queue.push(buffer);
  boost::asio::async_write(src_socket, boost::asio::buffer(buffer_data, length),
                           BIND_HANDLER_S(src_write_handler));
}

void ForwardingServerImpl::enqueue_dst_write(std::size_t length) {
  void *buffer_data = boost::asio::asio_handler_allocate(
      length, &BIND_HANDLER_S(dst_write_handler));
  std::cout << "[ allocated buffer 0x" << buffer_data << "[" << length << "]"
            << std::endl;
  // std::copy(src_read_buffer, src_read_buffer + length,
  // reinterpret_cast<char*>(buffer_data));
  std::memcpy(buffer_data, src_read_buffer, length);
  write_buffer_type buffer(buffer_data, length);
  dst_write_queue.push(buffer);
  boost::asio::async_write(dst_socket, boost::asio::buffer(buffer_data, length),
                           BIND_HANDLER_S(dst_write_handler));
}

} // namespace port
} // namespace bookfiler
