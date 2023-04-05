#include "mesh_def.h"
#include <vcg/complex/algorithms/create/marching_cubes.h>
#include <vcg/complex/algorithms/create/mc_trivial_walker.h>

#include <muParser.h>

using namespace vcg;
using namespace std;

namespace muml // muparser meshlab
{ 
double x,y,z,nx,ny,nz,r,g,b,a,q,rad,vtu,vtv,vsel;
double x0,y0,z0,x1,y1,z1,x2,y2,z2,nx0,ny0,nz0,nx1,ny1,nz1,nx2,ny2,nz2,r0,g0,b0,a0,r1,g1,b1,a1,r2,g2,b2,a2,q0,q1,q2,wtu0,wtv0,wtu1,wtv1,wtu2,wtv2,vsel0,vsel1,vsel2;
double fr,fg,fb,fa,fnx,fny,fnz,fq,fsel;
double v,f,v0i,v1i,v2i,ti;
std::vector<std::string> v_attrNames;  // names of the <float> per vertex attributes
std::vector<double>      v_attrValue;  // values of the <float> per vertex attributes
std::vector<std::string> v3_attrNames;  // names of the <Point3f> per vertex attributes There are 3x (one foreach coord _x, _y, _z)
std::vector<double>      v3_attrValue;  // values of the <Point3f> per vertex attributes. There are 3x (one foreach coord _x, _y, _z)
std::vector<std::string> f_attrNames;
std::vector<double> f_attrValue;
std::vector<MyMesh::PerVertexAttributeHandle<float> > v_handlers;
std::vector<MyMesh::PerVertexAttributeHandle<Point3f> > v3_handlers;
std::vector<MyMesh::PerFaceAttributeHandle<float> > f_handlers;


// Function explicitely define parser variables to perform per-vertex filter action
// x, y, z for vertex coord, nx, ny, nz for normal coord, r, g ,b for color
// and q for quality
void setPerVertexVariables(mu::Parser &p, MyMesh &m)
{
	p.DefineVar("x", &x);
	p.DefineVar("y", &y);
	p.DefineVar("z", &z);
	p.DefineVar("nx", &nx);
	p.DefineVar("ny", &ny);
	p.DefineVar("nz", &nz);
	p.DefineVar("r", &r);
	p.DefineVar("g", &g);
	p.DefineVar("b", &b);
	p.DefineVar("a", &a);
	p.DefineVar("q", &q);
	p.DefineVar("vi",&v);
	p.DefineVar("rad",&rad);
	p.DefineVar("vtu",&vtu);
	p.DefineVar("vtv",&vtv);
	p.DefineVar("ti", &ti);
	p.DefineVar("vsel", &vsel);

    // define var for user-defined attributes (if any exists)
    // if vector is empty, code won't be executed
	v_handlers.clear();
	v_attrNames.clear();
	v_attrValue.clear();
	v3_handlers.clear();
	v3_attrNames.clear();
	v3_attrValue.clear();
	std::vector<std::string> AllVertexAttribName;
	tri::Allocator<MyMesh>::GetAllPerVertexAttribute< float >(m,AllVertexAttribName);
	for(int i = 0; i < (int) AllVertexAttribName.size(); i++)
	{
		MyMesh::PerVertexAttributeHandle<float> hh = tri::Allocator<MyMesh>::GetPerVertexAttribute<float>(m, AllVertexAttribName[i]);
		v_handlers.push_back(hh);
		v_attrNames.push_back(AllVertexAttribName[i]);
		v_attrValue.push_back(0);
		p.DefineVar(v_attrNames.back(), &v_attrValue.back());
		printf("Adding custom per vertex float variable %s",v_attrNames.back().c_str());
	}
	AllVertexAttribName.clear();
	tri::Allocator<MyMesh>::GetAllPerVertexAttribute< Point3f >(m,AllVertexAttribName);
	for(int i = 0; i < (int) AllVertexAttribName.size(); i++)
	{
		MyMesh::PerVertexAttributeHandle<Point3f> hh3 = tri::Allocator<MyMesh>::GetPerVertexAttribute<Point3f>(m, AllVertexAttribName[i]);

		v3_handlers.push_back(hh3);

		v3_attrValue.push_back(0);
		v3_attrNames.push_back(AllVertexAttribName[i]+"_x");
		p.DefineVar(v3_attrNames.back(), &v3_attrValue.back());

		v3_attrValue.push_back(0);
		v3_attrNames.push_back(AllVertexAttribName[i]+"_y");
		p.DefineVar(v3_attrNames.back(), &v3_attrValue.back());

		v3_attrValue.push_back(0);
		v3_attrNames.push_back(AllVertexAttribName[i]+"_z");
		p.DefineVar(v3_attrNames.back(), &v3_attrValue.back());
		printf("Adding custom per vertex Point3f variable %s",v3_attrNames.back().c_str());
	}
}

// set per-vertex attributes associated to parser variables
void setAttributes(MyMesh::VertexIterator &vi, MyMesh &m)
{
  x = (*vi).P()[0]; // coord x
  y = (*vi).P()[1]; // coord y
  z = (*vi).P()[2]; // coord z

  nx = (*vi).N()[0]; // normal coord x
  ny = (*vi).N()[1]; // normal coord y
  nz = (*vi).N()[2]; // normal coord z

  r = (*vi).C()[0];  // color R
  g = (*vi).C()[1];  // color G
  b = (*vi).C()[2];  // color B
  a = (*vi).C()[3];  // color ALPHA

  q = (*vi).Q();     // quality

  vsel = ((*vi).IsS()) ? 1.0 : 0.0;    //selection

  if(tri::HasPerVertexRadius(m)) rad = (*vi).R();
  else rad=0;

  v = vi - m.vert.begin(); // zero based index of current vertex

  if(tri::HasPerVertexTexCoord(m))
  {
    vtu=(*vi).T().U();
    vtv=(*vi).T().V();
	ti = (*vi).T().N();
  }
  else { vtu=vtv=ti=0; }

  // if user-defined attributes exist (vector is not empty)
  //  set variables to explicit value obtained through attribute's handler
  for(int i = 0; i < (int) v_attrValue.size(); i++)
    v_attrValue[i] = v_handlers[i][vi];

  for(int i = 0; i < (int) v3_handlers.size(); i++)
  {
    v3_attrValue[i*3+0] = v3_handlers[i][vi].X();
    v3_attrValue[i*3+1] = v3_handlers[i][vi].Y();
    v3_attrValue[i*3+2] = v3_handlers[i][vi].Z();
  }
}

} // end namespace muml

bool QualityFunctionFilter(uintptr_t _baseM, std::string funcStr, bool normalizeFlag, bool colorMapFlag)
{
  MyMesh &m = *((MyMesh*) _baseM);
  printf("Applied Quality Function %s\n",funcStr.c_str());
  
  // muparser initialization and define custom variables
  mu::Parser p;
  muml::setPerVertexVariables(p,m);

  // set expression to calc with parser
  p.SetExpr(funcStr);

  // every parser variables is related to vertex coord and attributes.
  time_t start = clock();
  for(MyMesh ::VertexIterator vi = m.vert.begin(); vi != m.vert.end(); ++vi)
  {
          muml::setAttributes(vi,m);

          // use parser to evaluate function specified above
          // in case of fail, errorMessage dialog contains details of parser's error
          try {
            (*vi).Q() = p.Eval();
          } catch(mu::Parser::exception_type &e) {
            return false;
          }
        }

  if(normalizeFlag) tri::UpdateQuality<MyMesh>::VertexNormalize(m);
  if(colorMapFlag) tri::UpdateColor<MyMesh>::PerVertexQualityRamp(m);
  printf( "%d vertices processed in %.2f sec.", m.vn, (clock() - start) / (float) CLOCKS_PER_SEC);
  return true; 
}


bool ParametricSurfaceFilter(uintptr_t _baseM, std::string funcStrX, std::string funcStrY,  std::string funcStrZ, 
                             float minU, float maxU, int countU,
                             float minV, float maxV, int countV)
{
  MyMesh &m = *((MyMesh*) _baseM);
  
  float stepU = (maxU-minU)/float(countU);
  float stepV = (maxV-minV)/float(countV);
  double u,v;
  mu::Parser pX; pX.DefineVar("u", &u); pX.DefineVar("v", &v); pX.SetExpr(funcStrX);
  mu::Parser pY; pY.DefineVar("u", &u); pY.DefineVar("v", &v); pY.SetExpr(funcStrY);
  mu::Parser pZ; pZ.DefineVar("u", &u); pZ.DefineVar("v", &v); pZ.SetExpr(funcStrZ);
  
  for(int j=0; j<=countV; ++j)
    for(int i=0; i<=countU; ++i)
    {
      u = minU+i*stepU;
      v = minV+j*stepV;
      tri::Allocator<MyMesh>::AddVertex(m, Point3f(pX.Eval(),pY.Eval(),pZ.Eval()) );
    }
  
  tri::FaceGrid(m,countU+1,countV+1);
  m.UpdateBoxAndNormals();
  return true;
}


bool IsosurfaceFilter(uintptr_t _baseM, std::string funcStr, 
                     float minX, float minY, float minZ,
                     float maxX, float maxY, float maxZ,
                     float voxelSize) 
{
  MyMesh &m = *((MyMesh*) _baseM);
  
  SimpleVolume<SimpleVoxel <float > > 	volume;

  typedef vcg::tri::TrivialWalker<MyMesh, SimpleVolume<SimpleVoxel<float> > >	MyWalker;
  typedef vcg::tri::MarchingCubes<MyMesh, MyWalker>	MyMarchingCubes;
  MyWalker walker;

  Box3f RangeBBox;
  RangeBBox.min = Point3f(minX,minY,minZ);
  RangeBBox.max = Point3f(maxX,maxY,maxZ);
  Point3i siz= Point3i::Construct((RangeBBox.max-RangeBBox.min)*(1.0/voxelSize));

  mu::Parser p;
  double x,y,z;
  p.DefineVar("x", &x);
  p.DefineVar("y", &y);
  p.DefineVar("z", &z);
  p.SetExpr(funcStr);
  printf("Filling a Volume of %i %i %i",siz[0],siz[1],siz[2]);
  volume.Init(siz,RangeBBox);
  for(double i=0;i<siz[0];i++)
    for(double j=0;j<siz[1];j++)
      for(double k=0;k<siz[2];k++)
      {
        x = RangeBBox.min[0]+voxelSize*i;
        y = RangeBBox.min[1]+voxelSize*j;
        z = RangeBBox.min[2]+voxelSize*k;
        try {
          volume.Val(i,j,k)=p.Eval();
        } catch(mu::Parser::exception_type &e) {
          return false;
        }
      }

  // MARCHING CUBES
  printf("[MARCHING CUBES] Building mesh...");
  MyMarchingCubes					mc(m, walker);
  walker.BuildMesh<MyMarchingCubes>(m, volume, mc, 0);
  m.UpdateBoxAndNormals();
  return true;
}

void FuncParserPluginTEST()
{

}


#ifdef __EMSCRIPTEN__
//Binding code
EMSCRIPTEN_BINDINGS(MLFuncParserPlugin) {
    emscripten::function("QualityFunctionFilter", &QualityFunctionFilter);
    emscripten::function("ParametricSurfaceFilter", &ParametricSurfaceFilter);
    emscripten::function("IsosurfaceFilter", &IsosurfaceFilter);
    
}
#endif
