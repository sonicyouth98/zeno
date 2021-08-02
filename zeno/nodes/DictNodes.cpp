#include <zeno/zeno.h>
#include <zeno/types/DictObject.h>
#include <zeno/types/StringObject.h>
#include <zeno/types/NumericObject.h>
#include <zeno/utils/string.h>

namespace {

struct DictSize : zeno::INode {
    virtual void apply() override {
        auto dict = get_input<zeno::DictObject>("dict");
        auto ret = std::make_shared<zeno::NumericObject>();
        ret->set<int>(dict->lut.size());
        set_output("size", std::move(ret));
    }
};

ZENDEFNODE(DictSize, {
    {{"dict", "dict"}},
    {{"numeric:int", "size"}},
    {},
    {"dict"},
});


struct DictGetItem : zeno::INode {
    virtual void apply() override {
        auto dict = get_input<zeno::DictObject>("dict");
        auto key = get_input<zeno::StringObject>("key")->get();
        auto obj = dict->lut.at(key);
        set_output("object", std::move(obj));
    }
};

ZENDEFNODE(DictGetItem, {
    {{"dict", "dict"}, {"string", "key"}},
    {{"any", "object"}},
    {},
    {"dict"},
});


struct EmptyDict : zeno::INode {
    virtual void apply() override {
        auto dict = std::make_shared<zeno::DictObject>();
        set_output("dict", std::move(dict));
    }
};

ZENDEFNODE(EmptyDict, {
    {},
    {{"dict", "dict"}},
    {},
    {"dict"},
});


struct DictSetItem : zeno::INode {
    virtual void apply() override {
        auto dict = get_input<zeno::DictObject>("dict");
        auto key = get_input<zeno::StringObject>("key")->get();
        auto obj = get_input("object");
        dict->lut[key] = std::move(obj);
        set_output("dict", get_input("dict"));
    }
};

ZENDEFNODE(DictSetItem, {
    {{"dict", "dict"}, {"string", "key"}, {"any", "object"}},
    {{"dict", "dict"}},
    {},
    {"dict"},
});


struct MakeDict : zeno::INode {
    virtual void apply() override {
        auto inkeys = get_param<std::string>("_KEYS");
        auto keys = zeno::split_str(inkeys, '\n');
        auto dict = std::make_shared<zeno::DictObject>();
        for (auto const &key: keys) {
            if (has_input(key)) {
                auto obj = get_input(key);
                dict->lut[key] = std::move(obj);
            }
        }
        set_output("dict", std::move(dict));
    }
};

ZENDEFNODE(MakeDict, {
    {},
    {{"dict", "dict"}},
    {},
    {"dict"},
});


struct DictUnion : zeno::INode {
    virtual void apply() override {
        auto dict1 = get_input<zeno::DictObject>("dict1");
        auto dict2 = get_input<zeno::DictObject>("dict2");
        auto dict = std::make_shared<zeno::DictObject>();
        dict->lut = dict1->lut;
        dict->lut.merge(dict2->lut);
        set_output("dict", std::move(dict));
    }
};

ZENDEFNODE(DictUnion, {
    {{"dict1", "dict"}, {"dict2", "dict"}},
    {{"dict", "dict"}},
    {},
    {"dict"},
});



struct ExtractDict : zeno::INode {
    virtual void apply() override {
        auto inkeys = get_param<std::string>("_KEYS");
        auto keys = zeno::split_str(inkeys, '\n');
        auto dict = get_input<zeno::DictObject>("dict");
        for (auto const &key: keys) {
            auto it = dict->lut.find(key);
            if (it == dict->lut.end())
                continue;
            auto obj = it->second;
            set_output(key, std::move(obj));
        }
    }
};

ZENDEFNODE(ExtractDict, {
    {{"dict", "dict"}},
    {},
    {},
    {"dict"},
});

}
