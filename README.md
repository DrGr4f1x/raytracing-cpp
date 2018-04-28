# raytracing-cpp

This is my hobby ray tracer, built as a learning project.  I'm mainly following [Ray Tracing in One Weekend](https://www.amazon.com/Ray-Tracing-Weekend-Minibooks-Book-ebook/dp/B01B5AODD8/) by Peter Shirley, as well as the [blog series](http://aras-p.info/blog/2018/03/28/Daily-Pathtracer-Part-0-Intro/) by Aras P.  This is purely a learning study, since I've never actually written a ray tracer from scratch.

My goals are to explore some threading and SIMD optimizations, spatial data structures, and material models.  And of course to have some fun!

The ray tracer currently renders Lambertian, metallic, and dielectric spheres (no lighting), and generates the cover image on Peter Shirley's book.  I'm using my desktop PC and laptop for performance testing.
* Desktop specs: Intel i7 4790K (4.00 GHz), 32 GB RAM, NVidia 970 GPU.
* Laptop specs: MSI GS60 Ghost Pro, with Intel i7 6700HQ (2.60 GHz), 16 GB RAM, NVidia 970M GPU.

Build environment:
* Windows 10, Visual Studio 2017 15.6.5, C++ (x64)

## [01 Basic Ray Tracer](https://github.com/DrGr4f1x/raytracing-cpp/releases/tag/0.1-basic)
This is my initial checkpoint, totally unoptimized.  Here are current performance numbers (1280 x 720, 16 samples):

Machine | Primary rays per second | Total rays per second | Speedup (total rays)
------- | ----------------------- | --------------------- | --------------------
Desktop | 48.1 KRays / sec | 128.2 KRays / sec | 1.0x
Laptop | 38.8 KRays / sec | 104.1 KRays / sec | 1.0x

## [02 Threading](https://github.com/DrGr4f1x/raytracing-cpp/releases/tag/0.2-thread)
I added a threaded render loop, which generated the image in 8x8 pixel tiles, using parallel_for.  This results in approximately 3.4x speedup.

Machine | Primary rays per second | Total rays per second | Speedup (total rays)
------- | ----------------------- | --------------------- | --------------------
Desktop | 163.2 KRays / sec | 434.0 KRays / sec | 3.38x
Laptop | 133.1 KRays / sec | 354.0 KRays / sec | 3.4x

## [03 Embree](https://github.com/DrGr4f1x/raytracing-cpp/releases/tag/0.3-embree)
I added a reference renderer written with [Embree](https://embree.github.io/).  Below are performance numbers for the Embree version, which is substantially faster than mine.  I'll track both versions going forward, assuming I make meaningful changes to one or the other.

Machine | Primary rays per second | Total rays per second | Speedup (total rays)
------- | ----------------------- | --------------------- | --------------------
Desktop | 8.75 MRays / sec | 23.3 MRays / sec | 1.0x (53.7x)
Laptop | 6.99 MRays / sec | 18.6 MRays / sec | 1.0x (52.5x)

## [04 Iterative](https://github.com/DrGr4f1x/raytracing-cpp/releases/tag/0.4.1-iterative)
I suspected the recursive implementation of GetColor was probably a performance sink.  I added another implementation, called GetColor_Iterative, which as the name suggests, uses a while-loop instead of recursion.  There was a slight improvement in both the native and Embree versions, but it was not particularly significant.

**Native**

Machine | Primary rays per second | Total rays per second | Speedup (total rays)
------- | ----------------------- | --------------------- | --------------------
Desktop | 171.2 KRays / sec | 455.2 KRays / sec | 1.05x
Laptop | 134.7 KRays / sec | 358.0 KRays / sec | 1.01x

**Embree**

Machine | Primary rays per second | Total rays per second | Speedup (total rays)
------- | ----------------------- | --------------------- | --------------------
Desktop | 8.88 MRays / sec | 23.6 MRays / sec | 1.01x (51.8x)
Laptop | 7.34 MRays / sec | 19.5 MRays / sec | 1.05x (54.4x)

![Screenshot](/Screenshots/Image_16x.jpg?raw=true "Screenshot")
