#include "zeno/zeno.h"
#include "zeno/types/MaterialObject.h"
#include "zeno/types/PrimitiveObject.h"
#include "zeno/types/StringObject.h"

namespace zeno
{
  struct MakeMaterial
      : zeno::INode
  {
    virtual void apply() override
    {
      auto vert = get_input<zeno::StringObject>("vert")->get();
      auto frag = get_input<zeno::StringObject>("frag")->get();
      auto mtl = std::make_shared<zeno::MaterialObject>();

      if (vert.empty()) vert = R"(
#version 120

uniform mat4 mVP;
uniform mat4 mInvVP;
uniform mat4 mView;
uniform mat4 mProj;
uniform mat4 mInvView;
uniform mat4 mInvProj;

attribute vec3 vPosition;
attribute vec3 vColor;
attribute vec3 vNormal;

varying vec3 position;
varying vec3 iColor;
varying vec3 iNormal;

void main()
{
  position = vPosition;
  iColor = vColor;
  iNormal = vNormal;

  gl_Position = mVP * vec4(position, 1.0);
}
)";

      if (frag.empty()) frag = R"(
#version 120

uniform mat4 mVP;
uniform mat4 mInvVP;
uniform mat4 mView;
uniform mat4 mProj;
uniform mat4 mInvView;
uniform mat4 mInvProj;
uniform bool mSmoothShading;
uniform bool mRenderWireframe;

varying vec3 position;
varying vec3 iColor;
varying vec3 iNormal;

void main()
{
  gl_FragColor = vec4(8.0, 0.0, 0.0, 1.0);
}
)";

      mtl->vert = vert;
      mtl->frag = frag;
      set_output("mtl", std::move(mtl));
    }
  };

  ZENDEFNODE(
      MakeMaterial,
      {
          {
              {"string", "vert", ""},
              {"string", "frag", ""},
          },
          {
              {"material", "mtl"},
          },
          {},
          {
              "material",
          },
      });

  struct SetMaterial
      : zeno::INode
  {
    virtual void apply() override
    {
      auto prim = get_input<zeno::PrimitiveObject>("prim");
      auto mtl = get_input<zeno::MaterialObject>("mtl");
      prim->mtl = mtl;
      set_output("prim", std::move(prim));
    }
  };

  ZENDEFNODE(
      SetMaterial,
      {
          {
              {"primitive", "prim"},
              {"material", "mtl"},
          },
          {
              {"primitive", "prim"},
          },
          {},
          {
              "material",
          },
      });

} // namespace zeno
