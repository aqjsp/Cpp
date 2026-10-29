/**
 * @file epoll_server.hpp
 * @brief 基于 epoll 的高性能事件驱动网络服务器基础框架 (Linux Only)
 */

#pragma once

#if defined(__linux__)

#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <iostream>

namespace aq {

class EpollServer {
public:
    explicit EpollServer(int port, int max_events = 1024)
        : port_(port), max_events_(max_events) {
        
        listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd_ < 0) throw std::runtime_error("Socket failed");

        int opt = 1;
        setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        set_nonblocking(listen_fd_);

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port_);

        if (bind(listen_fd_, (sockaddr*)&addr, sizeof(addr)) < 0) 
            throw std::runtime_error("Bind failed");
        
        listen(listen_fd_, 128);

        epoll_fd_ = epoll_create1(0);
        if (epoll_fd_ < 0) throw std::runtime_error("Epoll create failed");

        epoll_event event{};
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = listen_fd_;
        epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, listen_fd_, &event);

        events_.resize(max_events_);
    }

    ~EpollServer() {
        close(listen_fd_);
        close(epoll_fd_);
    }

    void run() {
        std::cout << "Server listening on port " << port_ << "..." << std::endl;
        while (true) {
            int n = epoll_wait(epoll_fd_, events_.data(), max_events_, -1);
            for (int i = 0; i < n; ++i) {
                if (events_[i].data.fd == listen_fd_) {
                    handle_accept();
                } else {
                    handle_read(events_[i].data.fd);
                }
            }
        }
    }

private:
    void set_nonblocking(int fd) {
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

    void handle_accept() {
        while (true) {
            sockaddr_in client_addr;
            socklen_t len = sizeof(client_addr);
            int client_fd = accept(listen_fd_, (sockaddr*)&client_addr, &len);
            if (client_fd < 0) break;
            
            set_nonblocking(client_fd);
            epoll_event event{};
            event.events = EPOLLIN | EPOLLET;
            event.data.fd = client_fd;
            epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &event);
        }
    }

    void handle_read(int fd) {
        char buf[1024];
        while (true) {
            ssize_t n = read(fd, buf, sizeof(buf));
            if (n < 0) {
                if (errno != EAGAIN) {
                    close(fd);
                    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
                }
                break;
            } else if (n == 0) {
                close(fd);
                break;
            }
            // 简单的回显
            write(fd, buf, n);
        }
    }

    int port_;
    int listen_fd_;
    int epoll_fd_;
    int max_events_;
    std::vector<epoll_event> events_;
};

} // namespace aq

#endif
