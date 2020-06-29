#ifndef DRONE_SUBPROCESS_HPP
#define DRONE_SUBPROCESS_HPP

#define __posix__

#include <iostream>
#include <sstream>
#include <array>

#ifdef __posix__
#include <unistd.h>
#include <unistdio.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>
#endif

#ifdef __posix__
#include <cstring>
#include <cmath>
#endif

#include "../include/utility.hpp"

#ifdef __posix__
namespace drone::subprocess::posix {

    class posix_failure : public utility::context_exception {
    public:
        explicit posix_failure(std::string context) noexcept;

        template <class... TS>
        explicit posix_failure(const TS&... context_parts) noexcept;
    };

    posix_failure::posix_failure(std::string context) noexcept
            : context_exception(std::move(context)) {}

    template <class... TS>
    posix_failure::posix_failure(const TS&... context_parts) noexcept
            : context_exception(context_parts..., errno, std::strerror(errno)) {}


    void close_fd(int fd) {
        if (::close(fd) < 0)
            throw posix_failure("failed to close file descriptor:", fd);
    }

    void duplicate_fd(int old_fd, int new_fd) {
        if (::dup2(old_fd, new_fd) < 0)
            throw posix_failure("failed to duplicate file descriptor:", old_fd, new_fd);
    }

    void open_pipe_fds(int fds[2]) {
        if (::pipe(fds) < 0)
            throw posix_failure("failed to open pipe descriptors");
    }

    void close_pipe_fds(int fds[2]) {
        close_fd(fds[STDIN_FILENO]);
        close_fd(fds[STDOUT_FILENO]);
    }

    void kill_pid(int pid) {
        if (::kill(pid, SIGKILL) < 0)
            throw posix_failure("failed to kill process:", pid);
    }

    size_t read_fd(int fd, void* ptr, size_t n) {
        int rs = ::read(fd, ptr, n);

        if (rs < 0)
            throw posix_failure("failed to read file descriptor:", fd);

        return rs;
    }

    size_t write_fd(int fd, const void* ptr, size_t n) {
        int rs = ::write(fd, ptr, n);

        if (rs < 0)
            throw posix_failure("failed to write file descriptor:", fd);

        return rs;
    }

    int poll_fd(int fd, short events, int timeout = 100) {
        using pollfd_t = struct pollfd;

        pollfd_t pollfd[1] = {{fd, events, 0}};
        int rs = poll(pollfd, 1, timeout);

        if (rs < 0)
            throw posix_failure("failed to poll file descriptor:", fd);

        return rs;
    }


    template <class C, class T = std::char_traits<C>, size_t P = 32, size_t B = 512>
    class basic_posix_fd_istreambuf : public std::basic_streambuf<C, T> {
    public:
        using base = std::basic_streambuf<C, T>;

        using typename base::char_type;
        using typename base::int_type;

        using typename base::traits_type;

        explicit basic_posix_fd_istreambuf(int fd) noexcept;

    protected:
        static constexpr size_t _putback_size = P;
        static constexpr size_t _buffer_size = B;

        int_type underflow() override;

        std::array<char_type, _buffer_size + _putback_size> _buffer;
        int _fd;
    };

    template <class C, class T, size_t P, size_t B>
    basic_posix_fd_istreambuf<C, T, P, B>::basic_posix_fd_istreambuf(int fd) noexcept
        : _fd(fd) {}

    template <class C, class T, size_t P, size_t B>
    typename basic_posix_fd_istreambuf<C, T, P, B>::int_type basic_posix_fd_istreambuf<C, T, P, B>::underflow() {
        if (this->gptr() < this->egptr())
            return traits_type::to_int_type(*this->gptr());

        size_t putback_count = std::min((size_t) (this->gptr() - this->eback()), _putback_size);

        std::memmove(_buffer.data() + (_putback_size - putback_count),
                     this->gptr() - putback_count, putback_count);

        if (poll_fd(_fd, POLLIN) == 0)
            return traits_type::eof();

        size_t read_count = read_fd(_fd, _buffer.data() + _putback_size, _buffer_size);

        this->setg(_buffer.data() + _putback_size - putback_count,
                   _buffer.data() + _putback_size,
                   _buffer.data() + _putback_size + read_count);

        return traits_type::to_int_type(*this->gptr());
    }


    template <class C, class T = std::char_traits<C>>
    class basic_posix_fd_ostreambuf : public std::basic_streambuf<C, T> {
    public:
        using base = std::basic_streambuf<C, T>;

        using typename base::char_type;
        using typename base::int_type;

        using typename base::traits_type;

        explicit basic_posix_fd_ostreambuf(int fd) noexcept;

    protected:
        int_type overflow (int_type c) override;
        std::streamsize xsputn(const char* s, std::streamsize n) override;

    protected:
        int _fd;
    };

    template <class C, class T>
    basic_posix_fd_ostreambuf<C, T>::basic_posix_fd_ostreambuf(int fd) noexcept
        : _fd(fd) {}

    template <class C, class T>
    typename basic_posix_fd_ostreambuf<C, T>::int_type basic_posix_fd_ostreambuf<C, T>::overflow(int_type c) {
        if (c == traits_type::eof())
            return traits_type::eof();

        if (write_fd(_fd, &c, 1) != 1)
            return traits_type::eof();

        return c;
    }

    template <class C, class T>
    std::streamsize basic_posix_fd_ostreambuf<C, T>::xsputn(const char* s, std::streamsize n) {
        return write_fd(_fd, s, n);
    }


    template <class C, class T = std::char_traits<C>>
    class basic_posix_fd_istream : public std::basic_istream<C, T> {
    public:
        using base = std::basic_istream<C, T>;

        using typename base::char_type;
        using typename base::int_type;

        using typename base::traits_type;

        using buffer_type = basic_posix_fd_istreambuf<C, T>;

        explicit basic_posix_fd_istream(int fd) noexcept;


    protected:
        buffer_type _buffer;
    };

    template <class C, class T>
    basic_posix_fd_istream<C, T>::basic_posix_fd_istream(int fd) noexcept
        : base(nullptr), _buffer(fd)
    {
        this->rdbuf(&_buffer);
    }

    using posix_fd_istream = basic_posix_fd_istream<char>;


    template <class C, class T = std::char_traits<C>>
    class basic_posix_fd_ostream : public std::basic_ostream<C, T> {
    public:
        using base = std::basic_ostream<C, T>;

        using typename base::char_type;
        using typename base::int_type;

        using typename base::traits_type;

        using buffer_type = basic_posix_fd_ostreambuf<C, T>;

        explicit basic_posix_fd_ostream(int fd) noexcept;

    protected:
        buffer_type _buffer;
    };

    template <class C, class T>
    basic_posix_fd_ostream<C, T>::basic_posix_fd_ostream(int fd) noexcept
        : base(nullptr), _buffer(fd)
    {
        this->rdbuf(&_buffer);
    }

    using posix_fd_ostream = basic_posix_fd_ostream<char>;


    class posix_process {
    public:
        posix_process(const posix_process& process) = delete;
        ~posix_process();

        std::ostream& stdin();
        std::istream& stdout();
        std::istream& stderr();

        void close();
        void kill();

        template <class C, class... C_AS>
        static posix_process popen(const C& command, const C_AS&... arguments);

        template <class T>
        friend posix_process& operator>>(posix_process& process, T& object);

        template <class T>
        friend posix_process& operator<<(posix_process& process, const T& object);

    protected:
        explicit posix_process(int pid, int fdin, int fdout, int fderr);

        posix_fd_ostream _stdin;
        posix_fd_istream _stdout;
        posix_fd_istream _stderr;

        int _fdin, _fdout, _fderr;
        int _pid;

        bool _detached;
    };

    posix_process::posix_process(int pid, int fdin, int fdout, int fderr)
        :  _fdin(fdin),  _fdout(fdout),  _fderr(fderr),
          _stdin(fdin), _stdout(fdout), _stderr(fderr),
          _pid(pid),
          _detached(false) {}

    posix_process::~posix_process() {
        close();

        if (!_detached)
            kill();
    }

    std::ostream& posix_process::stdin() {
        return _stdin;
    }

    std::istream& posix_process::stdout() {
        return _stdout;
    }

    std::istream& posix_process::stderr() {
        return _stderr;
    }

    void posix_process::close() {
        close_fd(_fdin);
        close_fd(_fdout);
        close_fd(_fderr);
    }

    void posix_process::kill() {
        kill_pid(_pid);
    }

    template <class C = std::string, class... C_AS>
    posix_process posix_process::popen(const C& command, const C_AS&... arguments) {
        int stdin_pipe_fds[2], stdout_pipe_fds[2], stderr_pipe_fds[2];

        open_pipe_fds(stdin_pipe_fds);
        open_pipe_fds(stdout_pipe_fds);
        open_pipe_fds(stderr_pipe_fds);

        int pid = fork();

        if (pid < 0)
            throw posix_failure("failed to fork child process");

        if (pid > 0) {
            if (fcntl(stdin_pipe_fds[STDIN_FILENO], F_SETFL, O_NDELAY | O_NONBLOCK) < 0)
                throw posix_failure("failed to set flag for file descriptor");
            return posix_process(pid, stdin_pipe_fds[STDOUT_FILENO],
                                      stdout_pipe_fds[STDIN_FILENO],
                                      stderr_pipe_fds[STDIN_FILENO]);
        }

        close_fd(STDIN_FILENO);
        duplicate_fd(stdin_pipe_fds[STDIN_FILENO], STDIN_FILENO);

        close_fd(STDOUT_FILENO);
        duplicate_fd(stdout_pipe_fds[STDOUT_FILENO], STDOUT_FILENO);

        close_fd(STDERR_FILENO);
        duplicate_fd(stderr_pipe_fds[STDOUT_FILENO], STDERR_FILENO);

        close_pipe_fds(stdin_pipe_fds);
        close_pipe_fds(stdout_pipe_fds);
        close_pipe_fds(stderr_pipe_fds);

        if (execlp(command, arguments..., NULL)) {
            std::cerr << posix_failure("failed to execute process command").what() << std::endl;
            std::exit(EXIT_FAILURE);
        }

        std::exit(EXIT_SUCCESS);
    }

    template <class T>
    posix_process& operator>>(posix_process& process, T& object) {
        process._stdout >> object;
        return process;
    }

    template <class T>
    posix_process& operator<<(posix_process& process, const T& object) {
        process._stdin << object;
        return process;
    }
}

#endif

namespace drone::subprocess {

#ifdef __posix__
    using process = posix::posix_process;
#endif

}

#endif //DRONE_SUBPROCESS_HPP
