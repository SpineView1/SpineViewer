emcc --bind -s DEMANGLE_SUPPORT=1 -I ../../../../../vcglib/vcglib/ MeshLabJs.cpp Opener.cpp Saver.cpp Smooth.cpp Refine.cpp ../../../../../vcglib/vcglib/wrap/ply/plylib.cpp  -s TOTAL_MEMORY=268435456 -O3 -o ../js/generated/MeshlabJs.js