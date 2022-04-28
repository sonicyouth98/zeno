#include <openvdb/openvdb.h>
#include <cstdio>
namespace zeno {
namespace {
struct OpenvdbInitializer {
  OpenvdbInitializer() {
      printf("Initializing OpenVDB...\n");
      openvdb::initialize();
      printf("Initialized OpenVDB successfully!\n");
  }
};
static OpenvdbInitializer g_openvdb_initializer{};
}
}
