### Windows

Hello World:
```
g++ -g raytracer.cpp world/*.cpp utilities/*.cpp geometry/*.cpp cameras/*.cpp image/*.cpp samplers/*.cpp materials/*.cpp build/buildHelloWorld.cpp -o raytracer.exe
```

Mvp:
```
g++ -g raytracer.cpp world/*.cpp utilities/*.cpp geometry/*.cpp cameras/*.cpp image/*.cpp samplers/*.cpp materials/*.cpp build/buildMvp.cpp -o raytracer.exe
```

Chapter 14:
```
g++ -g raytracer.cpp world/*.cpp utilities/*.cpp geometry/*.cpp cameras/*.cpp image/*.cpp samplers/*.cpp materials/*.cpp build/buildChapter14.cpp -o raytracer.exe


### Mac
Hello World:
```
g++ -g raytracer.cpp world/*.cpp utilities/*.cpp geometry/*.cpp cameras/*.cpp image/*.cpp samplers/*.cpp materials/*.cpp build/buildHelloWorld.cpp -o raytracer.out
```

Mvp:
```
g++ -g raytracer.cpp world/*.cpp utilities/*.cpp geometry/*.cpp cameras/*.cpp image/*.cpp samplers/*.cpp materials/*.cpp build/buildMvp.cpp -o raytracer.out
```

Chapter 14:
```
g++ -g raytracer.cpp world/*.cpp utilities/*.cpp geometry/*.cpp cameras/*.cpp image/*.cpp samplers/*.cpp materials/*.cpp build/buildChapter14.cpp -o raytracer.out
```

BRDF:
```
g++ -g raytracer.cpp world/*.cpp utilities/*.cpp geometry/*.cpp cameras/*.cpp image/*.cpp samplers/*.cpp BRDF/*.cpp materials/*.cpp build/buildbrdf.cpp -o raytracer.out
```