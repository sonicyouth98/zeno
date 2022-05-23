#include <zeno/zeno.h>
#include <zeno/types/PrimitiveObject.h>
#include <zeno/types/DictObject.h>
#include <zeno/types/StringObject.h>
#include <zeno/types/PrimitiveTools.h>
#include <zeno/types/NumericObject.h>
#include <zeno/utils/string.h>
#include <zeno/utils/logger.h>
#include <zeno/utils/vec.h>
#define _USE_MATH_DEFINES
#include <math.h>
//#include <spdlog/spdlog.h>

namespace zeno {
struct CreateCube : zeno::INode {
    virtual void apply() override {
        auto prim = std::make_shared<zeno::PrimitiveObject>();
        auto size = get_input2<float>("size");
        auto position = get_input2<zeno::vec3f>("position");
        auto scaleSize = get_input2<zeno::vec3f>("scaleSize");

        auto &pos = prim->verts;
        pos.push_back(vec3f( 1,  1,  1) * size * scaleSize + position);
        pos.push_back(vec3f( 1,  1, -1) * size * scaleSize + position);
        pos.push_back(vec3f(-1,  1, -1) * size * scaleSize + position);
        pos.push_back(vec3f(-1,  1,  1) * size * scaleSize + position);
        pos.push_back(vec3f( 1, -1,  1) * size * scaleSize + position);
        pos.push_back(vec3f( 1, -1, -1) * size * scaleSize + position);
        pos.push_back(vec3f(-1, -1, -1) * size * scaleSize + position);
        pos.push_back(vec3f(-1, -1,  1) * size * scaleSize + position);

        auto &tris = prim->tris;
        // Top 0, 1, 2, 3
        tris.push_back(vec3i(0, 1, 2));
        tris.push_back(vec3i(0, 2, 3));
        // Right 0, 4, 5, 1
        tris.push_back(vec3i(0, 4, 5));
        tris.push_back(vec3i(0, 5, 1));
        // Front 0, 3, 7, 4
        tris.push_back(vec3i(0, 3, 7));
        tris.push_back(vec3i(0, 7, 4));
        // Left 2, 6, 7, 3
        tris.push_back(vec3i(2, 6, 7));
        tris.push_back(vec3i(2, 7, 3));
        // Back 1, 5, 6, 2
        tris.push_back(vec3i(1, 5, 6));
        tris.push_back(vec3i(1, 6, 2));
        // Bottom 4, 7, 6, 5
        tris.push_back(vec3i(4, 7, 6));
        tris.push_back(vec3i(4, 6, 5));
        set_output("prim", std::move(prim));
    }
};

ZENDEFNODE(CreateCube, {
    {
        {"vec3f", "position", "0, 0, 0"},
        {"vec3f", "scaleSize", "1, 1, 1"},
        {"float", "size", "1"},
    },
    {"prim"},
    {},
    {"create"},
});

struct CreateCone : zeno::INode {
    virtual void apply() override {
        auto prim = std::make_shared<zeno::PrimitiveObject>();
        auto position = get_input2<zeno::vec3f>("position");
        auto scaleSize = get_input2<zeno::vec3f>("scaleSize");
        auto radius = get_input2<float>("radius");
        auto height = get_input2<float>("height");
        auto lons = get_input2<int>("lons");

        auto &pos = prim->verts;
        for (size_t i = 0; i < lons; i++) {
            float rad = 2 * M_PI * i / lons;
            pos.push_back(vec3f(cos(rad) * radius, -0.5 * height, -sin(rad) * radius) * scaleSize + position);
        }
        // top
        pos.push_back(vec3f(0, 0.5 * height, 0) * scaleSize + position);
        // bottom
        pos.push_back(vec3f(0, -0.5 * height, 0) * scaleSize + position);

        auto &tris = prim->tris;
        for (size_t i = 0; i < lons; i++) {
            tris.push_back(vec3i(lons, i, (i + 1) % lons));
            tris.push_back(vec3i(i, lons + 1, (i + 1) % lons));
        }

        set_output("prim", std::move(prim));
    }
};

ZENDEFNODE(CreateCone, {
    {
        {"vec3f", "position", "0, 0, 0"},
        {"vec3f", "scaleSize", "1, 1, 1"},
        {"float", "radius", "1"},
        {"float", "height", "2"},
        {"int", "lons", "32"},
    },
    {"prim"},
    {},
    {"create"},
});

struct CreateDisk : zeno::INode {
    virtual void apply() override {
        auto prim = std::make_shared<zeno::PrimitiveObject>();
        auto position = get_input2<zeno::vec3f>("position");
        auto scaleSize = get_input2<zeno::vec3f>("scaleSize");
        auto radius = get_input2<float>("radius");
        auto lons = get_input2<int>("lons");

        auto &pos = prim->verts;
        for (size_t i = 0; i < lons; i++) {
            float rad = 2 * M_PI * i / lons;
            pos.push_back(vec3f(cos(rad) * radius, 0, -sin(rad) * radius) * scaleSize + position);
        }
        pos.push_back(vec3f(0, 0, 0) * scaleSize + position);

        auto &tris = prim->tris;
        for (size_t i = 0; i < lons; i++) {
            tris.push_back(vec3i(lons, i, (i + 1) % lons));
        }

        set_output("prim", std::move(prim));
    }
};

ZENDEFNODE(CreateDisk, {
    {
        {"vec3f", "position", "0, 0, 0"},
        {"vec3f", "scaleSize", "1, 1, 1"},
        {"float", "radius", "1"},
        {"int", "lons", "32"},
    },
    {"prim"},
    {},
    {"create"},
});

struct CreatePlane : zeno::INode {
    virtual void apply() override {
        auto prim = std::make_shared<zeno::PrimitiveObject>();
        auto position = get_input2<zeno::vec3f>("position");
        auto scaleSize = get_input2<zeno::vec3f>("scaleSize");
        auto size = get_input2<float>("size");

        auto &pos = prim->verts;
        pos.push_back(vec3f( 1, 0,  1) * size * scaleSize + position);
        pos.push_back(vec3f( 1, 0, -1) * size * scaleSize + position);
        pos.push_back(vec3f(-1, 0, -1) * size * scaleSize + position);
        pos.push_back(vec3f(-1, 0,  1) * size * scaleSize + position);

        auto &tris = prim->tris;
        tris.push_back(vec3i(0, 1, 2));
        tris.push_back(vec3i(0, 2, 3));

        set_output("prim", std::move(prim));
    }
};

ZENDEFNODE(CreatePlane, {
    {
        {"vec3f", "position", "0, 0, 0"},
        {"vec3f", "scaleSize", "1, 1, 1"},
        {"float", "size", "1"},
    },
    {"prim"},
    {},
    {"create"},
});

struct CreateCylinder : zeno::INode {
    virtual void apply() override {
        auto prim = std::make_shared<zeno::PrimitiveObject>();

        auto position = get_input2<zeno::vec3f>("position");
        auto scaleSize = get_input2<zeno::vec3f>("scaleSize");
        auto radius = get_input2<float>("radius");
        auto height = get_input2<float>("height");
        auto lons = get_input2<int>("lons");

        auto &pos = prim->verts;
        for (size_t i = 0; i < lons; i++) {
            float rad = 2 * M_PI * i / lons;
            pos.push_back(vec3f(cos(rad) * radius, 0.5 * height, -sin(rad) * radius) * scaleSize + position);
        }
        for (size_t i = 0; i < lons; i++) {
            float rad = 2 * M_PI * i / lons;
            pos.push_back(vec3f(cos(rad) * radius, -0.5 * height, -sin(rad) * radius) * scaleSize + position);
        }
        pos.push_back(vec3f(0, 0.5 * height, 0) * scaleSize + position);
        pos.push_back(vec3f(0, -0.5 * height, 0) * scaleSize + position);

        auto &tris = prim->tris;
        // Top
        for (size_t i = 0; i < lons; i++) {
            tris.push_back(vec3i(lons * 2, i, (i + 1) % lons));
        }
        // Bottom
        for (size_t i = 0; i < lons; i++) {
            tris.push_back(vec3i(i + lons, lons * 2 + 1, (i + 1) % lons + lons));
        }
        // Side
        for (size_t i = 0; i < lons; i++) {
            size_t _0 = i;
            size_t _1 = (i + 1) % lons;
            size_t _2 = (i + 1) % lons + lons;
            size_t _3 = i + lons;
            tris.push_back(vec3i(_1, _0, _2));
            tris.push_back(vec3i(_2, _0, _3));
        }
        set_output("prim", std::move(prim));
    }
};

ZENDEFNODE(CreateCylinder, {
    {
        {"vec3f", "position", "0, 0, 0"},
        {"vec3f", "scaleSize", "1, 1, 1"},
        {"float", "radius", "1"},
        {"float", "height", "2"},
        {"int", "lons", "32"},
    },
    {"prim"},
    {},
    {"create"},
});

struct CreateSphere : zeno::INode {
    virtual void apply() override {
        auto prim = std::make_shared<zeno::PrimitiveObject>();
        auto position = get_input2<zeno::vec3f>("position");
        auto scaleSize = get_input2<zeno::vec3f>("scaleSize");
        auto radius = get_input2<float>("radius");

        size_t seg = 32;

        std::vector<vec3f> uvs;
        uvs.reserve(19 * 33);
        auto &pos = prim->verts;
        auto &nrm = prim->add_attr<zeno::vec3f>("nrm");
        for (auto i = -90; i <= 90; i += 10) {
            float r = cos(i / 180.0 * M_PI);
            float h = sin(i / 180.0 * M_PI);
            for (size_t j = 0; j <= seg; j++) {
                float rad = 2 * M_PI * j / 32;
                pos.push_back(vec3f(cos(rad) * r, h, -sin(rad) * r) * radius * scaleSize + position);
                uvs.push_back(vec3f(j / 32.0, i / 90.0 * 0.5 + 0.5, 0));
                nrm.push_back(zeno::normalize(pos[pos.size()-1]));
            }
        }

        auto &tris = prim->tris;
        auto &uv0  = tris.add_attr<zeno::vec3f>("uv0");
        auto &uv1  = tris.add_attr<zeno::vec3f>("uv1");
        auto &uv2  = tris.add_attr<zeno::vec3f>("uv2");
        size_t count = 0;
        for (auto i = -90; i < 90; i += 10) {
            for (size_t i = 0; i < seg; i++) {
                size_t _0 = i + (seg + 1) * count;
                size_t _1 = i + 1 + (seg + 1) * count;
                size_t _2 = i + 1 + (seg + 1) * (count + 1);
                size_t _3 = i + (seg + 1) * (count + 1);
                tris.push_back(vec3i(_1, _0, _2));
                tris.attr<zeno::vec3f>("uv0").push_back(uvs[_1]);
                tris.attr<zeno::vec3f>("uv1").push_back(uvs[_0]);
                tris.attr<zeno::vec3f>("uv2").push_back(uvs[_2]);

                tris.push_back(vec3i(_2, _0, _3));
                tris.attr<zeno::vec3f>("uv0").push_back(uvs[_2]);
                tris.attr<zeno::vec3f>("uv1").push_back(uvs[_0]);
                tris.attr<zeno::vec3f>("uv2").push_back(uvs[_3]);
            }
            count += 1;
        }

        set_output("prim", std::move(prim));
    }
};

ZENDEFNODE(CreateSphere, {
    {
        {"vec3f", "position", "0, 0, 0"},
        {"vec3f", "scaleSize", "1, 1, 1"},
        {"float", "radius", "1"},
    },
    {"prim"},
    {},
    {"create"},
});

}
