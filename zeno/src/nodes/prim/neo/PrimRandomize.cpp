#include <zeno/zeno.h>
#include <zeno/types/StringObject.h>
#include <zeno/types/PrimitiveObject.h>
#include <zeno/funcs/PrimitiveUtils.h>
#include <zeno/types/NumericObject.h>
#include <zeno/utils/wangsrng.h>
#include <zeno/utils/variantswitch.h>
#include <zeno/utils/arrayindex.h>
#include <zeno/utils/orthonormal.h>
#include <zeno/para/parallel_for.h>
#include <zeno/utils/vec.h>
#include <zeno/utils/log.h>
#include <cstring>
#include <cstdlib>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace zeno {
namespace {

struct randtype_scalar01 {
    auto operator()(wangsrng &rng) const {
        float offs{rng.next_float()};
        return offs;
    }
};

struct randtype_scalar11 {
    auto operator()(wangsrng &rng) const {
        float offs{rng.next_float()};
        return offs * 2 - 1;
    }
};

struct randtype_cube01 {
    auto operator()(wangsrng &rng) const {
        vec3f offs{rng.next_float(), rng.next_float(), rng.next_float()};
        return offs;
    }
};

struct randtype_cube11 {
    auto operator()(wangsrng &rng) const {
        vec3f offs{rng.next_float(), rng.next_float(), rng.next_float()};
        return offs * 2 - 1;
    }
};

struct randtype_plane01 {
    auto operator()(wangsrng &rng) const {
        vec3f offs{rng.next_float(), rng.next_float(), 0};
        return offs;
    }
};

struct randtype_plane11 {
    auto operator()(wangsrng &rng) const {
        vec3f offs{rng.next_float() * 2 - 1, rng.next_float() * 2 - 1, 0};
        return offs;
    }
};

struct randtype_disk {
    auto operator()(wangsrng &rng) const {
        float r1 = rng.next_float();
        float r2 = rng.next_float();
        r1 = std::sqrt(r1);
        r2 *= M_PI * 2;
        vec3f offs{r1 * std::sin(r2), r1 * std::cos(r2), 0};
        return offs;
    }
};

struct randtype_cylinder {
    auto operator()(wangsrng &rng) const {
        float r1 = rng.next_float();
        float r2 = rng.next_float();
        r1 = r1 * 2 - 1;
        r2 *= M_PI * 2;
        vec3f offs{std::sin(r2), std::cos(r2), r1};
        return offs;
    }
};

struct randtype_ball {
    auto operator()(wangsrng &rng) const {
        float r1 = rng.next_float();
        float r2 = rng.next_float();
        float r3 = rng.next_float();
        r1 = r1 * 2 - 1;
        r2 *= M_PI * 2;
        r3 = std::cbrt(r3) * std::sqrt(1 - r1 * r1);
        vec3f offs{r3 * std::sin(r2), r3 * std::cos(r2), r1};
        return offs;
    }
};

struct randtype_semiball {
    auto operator()(wangsrng &rng) const {
        float r1 = rng.next_float();
        float r2 = rng.next_float();
        float r3 = rng.next_float();
        r2 *= M_PI * 2;
        r3 = std::cbrt(r3) * std::sqrt(1 - r1 * r1);
        vec3f offs{r3 * std::sin(r2), r3 * std::cos(r2), r1};
        return offs;
    }
};

struct randtype_sphere {
    auto operator()(wangsrng &rng) const {
        float r1 = rng.next_float();
        float r2 = rng.next_float();
        r1 = r1 * 2 - 1;
        r2 *= M_PI * 2;
        float r3 = std::sqrt(1 - r1 * r1);
        vec3f offs{r3 * std::sin(r2), r3 * std::cos(r2), r1};
        return offs;
    }
};

struct randtype_semisphere {
    auto operator()(wangsrng &rng) const {
        float r1 = rng.next_float();
        float r2 = rng.next_float();
        r2 *= M_PI * 2;
        float r3 = std::sqrt(1 - r1 * r1);
        vec3f offs{r3 * std::sin(r2), r3 * std::cos(r2), r1};
        return offs;
    }
};

using RandTypes = std::variant
    < randtype_scalar01
    , randtype_scalar11
    , randtype_cube01
    , randtype_cube11
    , randtype_plane01
    , randtype_plane11
    , randtype_disk
    , randtype_cylinder
    , randtype_ball
    , randtype_semiball
    , randtype_sphere
    , randtype_semisphere
>;

static std::string_view lutRandTypes[] = {
    "scalar01",
    "scalar11",
    "cube01",
    "cube11",
    "plane01",
    "plane11",
    "disk",
    "cylinder",
    "ball",
    "semiball",
    "sphere",
    "semisphere",
};

}

ZENO_API void primRandomize(PrimitiveObject *prim, std::string attr, std::string dirAttr, std::string randType, std::string combType, float scale, int seed) {
    auto randty = enum_variant<RandTypes>(array_index_safe(lutRandTypes, randType, "randType"));
    auto combIsAdd = boolean_variant(combType == "add");
    auto hasDirArr = boolean_variant(!dirAttr.empty());
    std::visit([&] (auto &&randty, auto combIsAdd, auto hasDirArr) {
        using T = std::invoke_result_t<decltype(randty), wangsrng &>;
        auto &arr = prim->add_attr<T>(attr);
        auto const &dirArr = hasDirArr ? prim->attr<vec3f>(dirAttr) : std::vector<vec3f>();
        parallel_for((size_t)0, arr.size(), [&] (size_t i) {
            wangsrng rng(seed, i);
            T offs = randty(rng) * scale;

            if constexpr (hasDirArr.value && std::is_same_v<T, vec3f>) {
                vec3f dir = dirArr[i], b1, b2;
                pixarONB(dir, b1, b2);
                offs = offs[0] * b1 + offs[1] * b2 + offs[2] * dir;
            }

            if constexpr (combIsAdd.value)
                arr[i] += offs;
            else
                arr[i] = offs;
        });
    }, randty, combIsAdd, hasDirArr);
}

namespace {

struct PrimRandomize : INode {
    virtual void apply() override {
        auto prim = get_input<PrimitiveObject>("prim");
        auto scale = get_input2<float>("scale");
        auto seed = get_input2<int>("seed");
        auto attr = get_input2<std::string>("attr");
        auto dirAttr = get_input2<std::string>("dirAttr");
        auto randType = get_input2<std::string>("randType");
        auto combType = get_input2<std::string>("combType");
        primRandomize(prim.get(), attr, dirAttr, randType, combType, scale, seed);
        set_output("prim", get_input("prim"));
    }
};

ZENDEFNODE(PrimRandomize, {
    {
    {"PrimitiveObject", "prim"},
    {"string", "attr", "pos"},
    {"string", "dirAttr", ""},
    {"float", "scale", "1"},
    {"int", "seed", "0"},
    {"enum scalar01 scalar11 cube01 cube11 plane01 plane11 disk cylinder ball semiball sphere semisphere", "randType", "scalar01"},
    {"enum set add", "combType", "add"},
    },
    {
    {"PrimitiveObject", "prim"},
    },
    {
    },
    {"primitive"},
});

}
}
