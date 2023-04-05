#include "mesh_def.h"
#include "FilterTest.h"

using namespace vcg;
using namespace std;

bool IsWaterTight(MyMesh &m)
{
  if(m.vn==0) return false;
  tri::UpdateTopology<MyMesh>::FaceFace(m);
  int n = tri::Clean<MyMesh>::CountNonManifoldEdgeFF(m);
  if(n!=0) return false;

  return true;
}

int main(int /*argc*/, char*/*argv*/[])
{
#ifdef _POISSON_TEST_   
  PoissonPluginTEST();
#endif
  
#ifdef _MUPARSER_TEST_   
  FuncParserPluginTEST();
#endif
  CreatePluginTEST();
  MeshingPluginTEST();
  MeasurePluginTEST();
  RefinePluginTEST();
  SamplingPluginTEST();
  SelectionPluginTEST();
  TransformPluginTEST();
  printf("Done");
  return 0;
}
