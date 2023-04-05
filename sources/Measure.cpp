#include "mesh_def.h"
#include <vcg/complex/algorithms/inertia.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/point_sampling.h>
#include<vcg/complex/algorithms/voronoi_volume_sampling.h>
#include<vcg/complex/algorithms/skeleton.h>


using namespace vcg;
using namespace std;

void ComputeGeometricMeasures(uintptr_t meshPtr)
{
    MyMesh &m = *((MyMesh*) meshPtr);

    printf("Geometric measures:\n");

    tri::UpdateBounding<MyMesh>::Box(m);
    printf("Mesh Bounding Box size %f %f %f\n", m.bbox.DimX(), m.bbox.DimY(), m.bbox.DimZ());
    printf("Mesh Bounding Box diag %f\n", m.bbox.Diag());

    tri::Inertia<MyMesh> I(m);
    float volume = I.Mass();
    float area = tri::Stat<MyMesh>::ComputeMeshArea(m);
    printf("Mesh volume %f\n", volume);
    printf("Mesh surface is %f\n", area);

    Point3f bc = tri::Stat<MyMesh>::ComputeShellBarycenter(m);
    printf("Thin shell barycenter %9.6f  %9.6f  %9.6f\n", bc[0], bc[1], bc[2]);

    if (volume <= 0) {
        printf("Volume is negative, Mesh is not 'solid' (non manifold, non watertight, non consistently oriented)\n"
               "No information on barycenter and inertia tensor can be reported\n");
    } else {
        printf("Center of mass is %f %f %f\n", I.CenterOfMass()[0], I.CenterOfMass()[1], I.CenterOfMass()[2]);
        Matrix33f it;
        I.InertiaTensor(it);
        printf("Inertia tensor is: \n");
        printf("  | %9.6f  %9.6f  %9.6f |\n", it[0][0], it[0][1], it[0][2]);
        printf("  | %9.6f  %9.6f  %9.6f |\n", it[1][0], it[1][1], it[1][2]);
        printf("  | %9.6f  %9.6f  %9.6f |\n", it[2][0], it[2][1], it[2][2]);

        Matrix33f pca;
        Point3f pcav;
        I.InertiaTensorEigen(pca, pcav);
        printf("Principal axes are :\n");
        printf("  | %9.6f  %9.6f  %9.6f |\n", pca[0][0], pca[0][1], pca[0][2]);
        printf("  | %9.6f  %9.6f  %9.6f |\n", pca[1][0], pca[1][1], pca[1][2]);
        printf("  | %9.6f  %9.6f  %9.6f |\n", pca[2][0], pca[2][1], pca[2][2]);

        printf("Axes momenta are :\n");
        printf("  | %9.6f  %9.6f  %9.6f |\n", pcav[0], pcav[1], pcav[2]);
    }
}

void ComputeTopologicalMeasures(uintptr_t meshPtr)
{
    MyMesh &m = *((MyMesh*) meshPtr);

    printf("Topological measures:\n");
    tri::UpdateTopology<MyMesh>::FaceFace(m);

    int nonManifEdgeNumFF = tri::Clean<MyMesh>::CountNonManifoldEdgeFF(m, true);
    int faceEdgeNonManif = tri::UpdateSelection<MyMesh>::FaceCount(m);
    tri::UpdateSelection<MyMesh>::VertexClear(m);
    tri::UpdateSelection<MyMesh>::FaceClear(m);

    int nonManifVertNum = tri::Clean<MyMesh>::CountNonManifoldVertexFF(m, true);
    tri::UpdateSelection<MyMesh>::FaceFromVertexLoose(m);
    int faceVertNonManif = tri::UpdateSelection<MyMesh>::FaceCount(m);
    int edgeNum = 0, borderNum = 0, nonManifEdgeNum = 0;
    tri::Clean<MyMesh>::CountEdgeNum(m, edgeNum, borderNum, nonManifEdgeNum);
    printf("V: %6i E: %6i F:%6i\n", m.vn, edgeNum, m.fn);
    int unrefVertNum = tri::Clean<MyMesh>::CountUnreferencedVertex(m);
    printf("Unreferenced vertices %i\n", unrefVertNum);
    printf("Boundary edges %i\n", borderNum);

    int connectedComponentsNum = tri::Clean<MyMesh>::CountConnectedComponents(m);
    printf("Mesh is composed by %i connected component(s)\n", connectedComponentsNum);

    if (nonManifEdgeNumFF == 0 && nonManifVertNum == 0) {
        printf("Mesh is two-manifold\n");
    }

    if (nonManifEdgeNumFF != 0) printf("Mesh has %i non two manifold edges and %i faces are incident on these edges\n", nonManifEdgeNumFF, faceEdgeNonManif);

    if (nonManifVertNum != 0) printf("Mesh has %i non two manifold vertexes and %i faces are incident on these vertices\n", nonManifVertNum, faceVertNonManif);

    // For Manifold meshes compute some other stuff
    if (nonManifVertNum == 0 && nonManifEdgeNumFF == 0) {
        int holeNum = tri::Clean<MyMesh>::CountHoles(m);
        printf("Mesh has %i holes\n", holeNum);
        int genus = tri::Clean<MyMesh>::MeshGenus(m.vn-unrefVertNum, edgeNum, m.fn, holeNum, connectedComponentsNum);
        printf("Genus is %i\n", genus);
    } else {
        printf("Mesh has a undefined number of holes (non 2-manifold mesh)\n");
        printf("Genus is undefined (non 2-manifold mesh)\n");
    }
    tri::UpdateSelection<MyMesh>::VertexClear(m);
    tri::UpdateSelection<MyMesh>::FaceClear(m);
}

void ComputeHausdorffDistance(uintptr_t srcPtr, uintptr_t trgPtr, uintptr_t _sampleMeshPtr, int sampleNum, float distUpperBound)
{
  MyMesh &src = *((MyMesh*) srcPtr);
  MyMesh &trg = *((MyMesh*) trgPtr);
  MyMesh *sampleMeshPtr = ((MyMesh*) _sampleMeshPtr);

  HausdorffSampler<MyMesh> hs(&trg);
  hs.dist_upper_bound = distUpperBound;
  hs.init(sampleMeshPtr,0);
  printf("Sampled  mesh has %7i vert %7i face\n",src.vn,src.fn);
  printf("Searched mesh has %7i vert %7i face\n",trg.vn,trg.fn);
  printf("Max sampling distance %f on a bbox diag of %f\n",distUpperBound,trg.bbox.Diag());

  tri::SurfaceSampling<MyMesh,HausdorffSampler<MyMesh> >::VertexUniform(src,hs,sampleNum);
  tri::SurfaceSampling<MyMesh,HausdorffSampler<MyMesh> >::Montecarlo(src,hs,sampleNum);

  printf("Hausdorff Distance computed\n");
  printf("     Sampled %i pts (rng: 0)  on %s searched closest on %s \n",hs.n_total_samples,src.meshName.c_str(),trg.meshName.c_str());
  printf("     min : %f   max %f   mean : %f   RMS : %f \n",hs.getMinDist(),hs.getMaxDist(),hs.getMeanDist(),hs.getRMSDist());
  float d = src.bbox.Diag();
  printf("Values w.r.t. BBox Diag (%f)\n",d);
  printf("     min : %f   max %f   mean : %f   RMS : %f\n",hs.getMinDist()/d,hs.getMaxDist()/d,hs.getMeanDist()/d,hs.getRMSDist()/d);

  if(sampleMeshPtr)
  {
    printf("Created Sample Mesh with %i samples\n",sampleMeshPtr->vn);
  }
}

void ComputeQualityFromCurvature(uintptr_t meshPtr, int curvatureMode)
{
    MyMesh &m = *((MyMesh*) meshPtr);    
    tri::UpdateCurvature<MyMesh>::MeanAndGaussian(m);
    if(curvatureMode==0)
    {
      tri::UpdateQuality<MyMesh>::VertexFromGaussianCurvatureHG(m);
      printf("Computed Gaussian Curvature\n");
    }
    else
    {
      tri::UpdateQuality<MyMesh>::VertexFromMeanCurvatureHG(m);
      printf("Computed Mean Curvature\n");
    }
}

bool ComputeThickness(uintptr_t meshPtr, uintptr_t trgPtr, uintptr_t volPtr, uintptr_t skelPtr,
                      int montecarloSampleNum, float surfSamplingRadius, float distThr)
{
  MyMesh &baseM = *((MyMesh*) meshPtr);
  MyMesh *surfSamp = ((MyMesh*) trgPtr);
  MyMesh  *volSamp = ((MyMesh*) volPtr);
  MyMesh   *skelM = ((MyMesh*) skelPtr);
  if(!tri::Clean<MyMesh>::IsWaterTight(baseM))
  {
    printf("Volume Sampling Requires Watertight Mesh. Nothing Done.\n");
    return false;
  } 
  tri::UpdateBounding<MyMesh>::Box(baseM);
  float meshVol = tri::Stat<MyMesh>::ComputeMeshVolume(baseM);
  float boxVol = baseM.bbox.Volume();
  printf("Volume ratio %f = %f/%f \n", meshVol/boxVol,meshVol,boxVol);
  
  tri::VoronoiVolumeSampling<MyMesh> vvs(baseM);
  float poissonRadiusSurface = baseM.bbox.Diag()*surfSamplingRadius;
  int t0=clock();
  vvs.Init(poissonRadiusSurface);  
  int t1=clock();
  vvs.BuildMontecarloVolumeSampling(montecarloSampleNum);
  int t2=clock();
  tri::SampledSkeleton<MyMesh> ssk(vvs);
  ssk.ThicknessEvaluator(distThr,16,3,skelM);
  int t3=clock();
  printf("Surface Sampling %5.2f sec\n",float(t1-t0)/CLOCKS_PER_SEC);
  printf("Volume  Sampling %5.2f sec\n",float(t2-t1)/CLOCKS_PER_SEC);
  printf("Medial Axis Eval %5.2f sec\n",float(t3-t2)/CLOCKS_PER_SEC);
  tri::UpdateColor<MyMesh>::PerVertexQualityRamp(baseM);
  if(surfSamp) 
  {
//    tri::Append<MyMesh,MyMesh>::MeshCopy(*surfSamp, vvs.poissonSurfaceMesh);
    tri::UpdateColor<MyMesh>::PerVertexQualityRamp(*surfSamp);
  }
  if(volSamp)
  {
    tri::Append<MyMesh,MyMesh>::MeshCopy(*volSamp, vvs.montecarloVolumeMesh);
    tri::UpdateColor<MyMesh>::PerVertexQualityRamp(*volSamp);
  }
  return true;  
}  

void MeasurePluginTEST()
{
  MyMesh m0,m1;
  tri::Sphere(m0,3);
  tri::Sphere(m1,4);

  ComputeHausdorffDistance(uintptr_t(&m0),uintptr_t(&m1),0,10000,0.1);
  ComputeHausdorffDistance(uintptr_t(&m1),uintptr_t(&m0),0,10000,0.1);
  ComputeQualityFromCurvature(uintptr_t(&m0),0);
  ComputeQualityFromCurvature(uintptr_t(&m0),1);
  ComputeThickness(uintptr_t(&m0),0,0,0,10000,0.01,1.05);
  ComputeThickness(uintptr_t(&m1),0,0,0,10000,0.01,1.05);
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_BINDINGS(MLMeasurePlugin) {
    emscripten::function("ComputeGeometricMeasures",   &ComputeGeometricMeasures);
    emscripten::function("ComputeTopologicalMeasures", &ComputeTopologicalMeasures);
    emscripten::function("ComputeHausdorffDistance",   &ComputeHausdorffDistance);
    emscripten::function("ComputeQualityFromCurvature",   &ComputeQualityFromCurvature);
    emscripten::function("ComputeThickness",   &ComputeThickness);
}
#endif
