# Build instructions
Building requires gcc (not clang). Use g++-15 (or whichever version you have installed).

# Build Commands
Hello World:
```
g++-15 -O3 -fopenmp -g raytracer.cpp world/*.cpp utilities/*.cpp geometry/*.cpp cameras/*.cpp image/*.cpp samplers/*.cpp BRDF/*.cpp materials/*.cpp lights/*.cpp tracers/*.cpp acceleration/*.cpp build/buildHelloWorld.cpp -o hello.out
```

BRDFs:
```
g++-15 -O3 -fopenmp -g raytracer.cpp world/*.cpp utilities/*.cpp geometry/*.cpp cameras/*.cpp image/*.cpp samplers/*.cpp BRDF/*.cpp materials/*.cpp lights/*.cpp tracers/*.cpp acceleration/*.cpp build/buildbrdf.cpp -o brdf.out
```

Lights:
```
g++-15 -O3 -fopenmp -g raytracer.cpp world/*.cpp utilities/*.cpp geometry/*.cpp cameras/*.cpp image/*.cpp samplers/*.cpp BRDF/*.cpp materials/*.cpp lights/*.cpp tracers/*.cpp acceleration/*.cpp build/buildlights.cpp -o lights.out
```

Anti-Aliasing:
```
g++-15 -O3 -fopenmp -g raytracer.cpp world/*.cpp utilities/*.cpp geometry/*.cpp cameras/*.cpp image/*.cpp samplers/*.cpp BRDF/*.cpp materials/*.cpp lights/*.cpp tracers/*.cpp acceleration/*.cpp build/buildAntiAliasing.cpp -o AntiAliasing.out
```

Shadows:
```
g++-15 -O3 -fopenmp -g raytracer.cpp world/*.cpp utilities/*.cpp geometry/*.cpp cameras/*.cpp image/*.cpp samplers/*.cpp BRDF/*.cpp materials/*.cpp lights/*.cpp tracers/*.cpp acceleration/*.cpp build/buildShadow.cpp -o shadow.out
```

Path Tracing:
```
g++-15 -O3 -fopenmp -g raytracer.cpp world/*.cpp utilities/*.cpp geometry/*.cpp cameras/*.cpp image/*.cpp samplers/*.cpp BRDF/*.cpp materials/*.cpp lights/*.cpp tracers/*.cpp acceleration/*.cpp build/buildPathTrace.cpp -o pathtrace.out
```

Menger Sponge:
```
g++-15 -O3 -fopenmp -g raytracer.cpp world/*.cpp utilities/*.cpp geometry/*.cpp cameras/*.cpp image/*.cpp samplers/*.cpp BRDF/*.cpp materials/*.cpp lights/*.cpp tracers/*.cpp acceleration/*.cpp build/buildDeath.cpp -o deathtracer.out
```