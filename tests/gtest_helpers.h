#pragma once

#include <gtest/gtest.h>
#include <zeno/ztd/vec.h>
#include <zeno/ztd/type_info.h>

ZENO_NAMESPACE_BEGIN
namespace ztd::mathvec {

template <size_t N, class T, class Stream>
    requires (ztd::tuple_contains<Stream, std::tuple<std::stringstream, std::ostream>>::value)
Stream &operator<<(Stream &os, ztd::vec<N, T> const &v) {
    os << "ztd::vec<" << N << ", " << cpp_type_name(typeid(T)) << ">(";
    os << v[0];
    for (int i = 1; i < N; i++) {
        os << ", " << v[i];
    }
    os << ")";
    return os;
}

#define SHOW_VAR(x, ...) #x " = " << (x)

}
ZENO_NAMESPACE_END
