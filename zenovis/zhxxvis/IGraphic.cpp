#include "main.hpp"
#include "IGraphic.hpp"
#include "zenvisapi.hpp"
#include <zeno/types/LightObject.h>
#include <zeno/types/PrimitiveObject.h>


namespace zenvis {

/* std::vector<std::unique_ptr<FrameData>> frames; */

std::unique_ptr<IGraphic> makeGraphicPrimitive
    ( zeno::PrimitiveObject *prim
    , std::string const &path
    );
#ifdef ZENVIS_WITH_OPENVDB
std::unique_ptr<IGraphic> makeGraphicVolume
    ( std::string const &path
    );
#endif



namespace {
struct GraLight : IGraphic {
    static inline int lightCounter = 0;
    static inline std::vector<int> lightRevamp; // revamp[physical] = logical
    int lid;

    GraLight(zeno::LightObject *l) {
        lid = lightCounter++;
        int plid(lightRevamp.size());
        lightRevamp.push_back(lid);

        zenvis::setLightData(plid,
                {l->lightDir[0], l->lightDir[1], l->lightDir[2]},
                l->lightHight, l->shadowSoftness,
                {l->shadowTint[0], l->shadowTint[1], l->shadowTint[2]},
                {l->lightColor[0], l->lightColor[1], l->lightColor[2]},
                l->intensity, l->lightScale, l->isEnabled
                );
    }

    GraLight(GraLight &&) = delete;

  virtual void draw(bool reflect, float depthPass) override{};
  virtual void drawShadow(Light *light) override{};

    ~GraLight() {
        int plid(std::find(lightRevamp.begin(), lightRevamp.end(), lid) - lightRevamp.begin());
        zenvis::removeLight(plid);
        lightRevamp.erase(lightRevamp.begin() + plid);
    }
};
}


static std::unique_ptr<IGraphic> makeGraphic(zeno::IObject *obj) {
    std::string path = "/unused/param";  // never mind
    if (auto p = dynamic_cast<zeno::PrimitiveObject *>(obj)) {
        return makeGraphicPrimitive(p, path);

#ifdef ZENVIS_WITH_OPENVDB
    } else if (auto p = dynamic_cast<zeno::VDBGrid *>(obj)) {
        return makeGraphicVolume(path);
#endif

    } else if (auto p = dynamic_cast<zeno::LightObject *>(obj)) {
        return std::make_unique<GraLight>(p);

    } else {
        //printf("%s\n", ext.c_str());
        //assert(0 && "bad file extension name");
    }
    return nullptr;
}

FrameData *current_frame_data() {
    static FrameData currFrameData;
    return &currFrameData;
}

void auto_gc_frame_data(int nkeep) {
}

std::vector<int> get_valid_frames_list() {
    return {};
}

void clear_graphics() {
    current_frame_data()->graphics.clear();
}

/*void load_file(std::string name, std::string ext, std::string path, int frameid) {
    if (ext == ".lock")
        return;

    auto &graphics = current_frame_data()->graphics;
    if (graphics.find(name) != graphics.end()) {
        //printf("cached: %p %s %s\n", &graphics, path.c_str(), name.c_str());
        return;
    }
    //printf("load_file: %p %s %s\n", &graphics, path.c_str(), name.c_str());

    auto ig = makeGraphic(path, ext);
    if (!ig) return;
    graphics[name] = std::move(ig);
}*/

void zxx_load_object(std::string const &key, zeno::IObject *obj) {
    auto p = makeGraphic(obj);
    if (p) current_frame_data()->graphics[key] = std::move(p);
}

void zxx_delete_object(std::string const &key) {
    current_frame_data()->graphics.erase(key);
}


}
