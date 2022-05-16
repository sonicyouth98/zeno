#pragma once

#include <zeno/utils/api.h>
#include <string_view>
#include <typeinfo>
#include <string>
#include <memory>

namespace zeno {

struct Error {
    std::string message;

    ZENO_API explicit Error(std::string_view message) noexcept;
    ZENO_API virtual ~Error() noexcept;
    ZENO_API std::string const &what() const noexcept;

    Error(Error const &) = delete;
    Error &operator=(Error const &) = delete;
    Error(Error &&) = delete;
    Error &operator=(Error &&) = delete;
};

struct StdError : Error {
    std::exception_ptr eptr;

    ZENO_API explicit StdError(std::exception_ptr &&eptr) noexcept;
    ZENO_API ~StdError() noexcept override;
};

struct TypeError : Error {
    std::type_info const &expect;
    std::type_info const &got;
    std::string hint;

    ZENO_API explicit TypeError(std::type_info const &expect, std::type_info const &got, std::string_view hint) noexcept;
    ZENO_API ~TypeError() noexcept override;
};

struct KeyError : Error {
    std::string key;
    std::string hint;

    ZENO_API explicit KeyError(std::string_view key, std::string_view hint) noexcept;
    ZENO_API ~KeyError() noexcept override;
};

class ErrorException : public std::exception {
    std::shared_ptr<Error> const err;

public:
    ZENO_API explicit ErrorException(std::shared_ptr<Error> &&err) noexcept;
    ZENO_API ~ErrorException() noexcept override;
    ZENO_API char const *what() const noexcept override;
    ZENO_API std::shared_ptr<Error> getError() const noexcept;

    ErrorException(ErrorException const &) = default;
    ErrorException &operator=(ErrorException const &) = delete;
    ErrorException(ErrorException &&) = default;
    ErrorException &operator=(ErrorException &&) = delete;
};

template <class T = Error, class ...Ts>
static ErrorException makeError(Ts &&...ts) {
    return ErrorException(std::make_shared<T>(std::forward<Ts>(ts)...));
}

}
