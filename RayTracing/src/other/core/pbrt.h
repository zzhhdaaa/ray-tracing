#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_CORE_PBRT_H
#define PBRT_CORE_PBRT_H

// Global Include Files
#include <type_traits>
#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
//#include "error.h"
#ifdef PBRT_HAVE_MALLOC_H
#include <malloc.h>  // for _alloca, memalign
#endif
#ifdef PBRT_HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include <assert.h>
#include <string.h>
//#include <glog/logging.h>

// Global Forward Declarations
class Scene;
class Integrator;
class SamplerIntegrator;
template <typename T>
class Vector2;
template <typename T>
class Vector3;
template <typename T>
class Point3;
template <typename T>
class Point2;
template <typename T>
class Normal3;
class Ray;
class RayDifferential;
template <typename T>
class Bounds2;
template <typename T>
class Bounds3;
class Transform;
struct Interaction;
class SurfaceInteraction;
class Shape;
class Primitive;
class GeometricPrimitive;
class TransformedPrimitive;
template <int nSpectrumSamples>
class CoefficientSpectrum;
class RGBSpectrum;
class SampledSpectrum;
#ifdef PBRT_SAMPLED_SPECTRUM
typedef SampledSpectrum Spectrum;
#else
typedef RGBSpectrum Spectrum;
#endif
class Camera;
struct CameraSample;
class ProjectiveCamera;
class Sampler;
class Filter;
class Film;
class FilmTile;
class BxDF;
class BRDF;
class BTDF;
class BSDF;
class Material;
template <typename T>
class Texture;
class Medium;
class MediumInteraction;
struct MediumInterface;
class BSSRDF;
class SeparableBSSRDF;
class TabulatedBSSRDF;
struct BSSRDFTable;
class Light;
class VisibilityTester;
class AreaLight;
struct Distribution1D;
class Distribution2D;
#ifdef PBRT_FLOAT_AS_DOUBLE
typedef double Float;
#else
typedef float Float;
#endif  // PBRT_FLOAT_AS_DOUBLE
class RNG;
class ProgressReporter;
class MemoryArena;
template <typename T, int logBlockSize = 2>
class BlockedArray;
struct Matrix4x4;
class ParamSet;
template <typename T>
struct ParamSetItem;
struct Options
{
	Options()
	{
		cropWindow[0][0] = 0;
		cropWindow[0][1] = 1;
		cropWindow[1][0] = 0;
		cropWindow[1][1] = 1;
	}
	int nThreads = 0;
	bool quickRender = false;
	bool quiet = false;
	bool cat = false, toPly = false;
	std::string imageFile;
	// x0, x1, y0, y1
	Float cropWindow[2][2];
};

// Global Constants
static const Float Pi = 3.14159265358979323846;
static const Float InvPi = 0.31830988618379067154;
static const Float Inv2Pi = 0.15915494309189533577;
static const Float Inv4Pi = 0.07957747154594766788;
static const Float PiOver2 = 1.57079632679489661923;
static const Float PiOver4 = 0.78539816339744830961;
static const Float Sqrt2 = 1.41421356237309504880;

// Global Inline Functions
template <typename T, typename U, typename V> inline T Clamp(T val, U low, V high)
{
	if (val < low) return low;
	else if (val > high) return high;
	else return val;
}

template <typename T> inline T Mod(T a, T b)
{
	T result = a - (a / b) * b;
	return (T)((result < 0) ? result + b : result);
}

template <> inline Float Mod(Float a, Float b)
{
	return std::fmod(a, b);
}

inline Float Radians(Float deg)
{
	return (Pi / 180) * deg;
}

inline Float Degrees(Float rad)
{
	return (180 / Pi) * rad;
}

inline Float Log2(Float x)
{
	const Float invLog2 = 1.442695040888963387004650940071;
	return std::log(x) * invLog2;
}

inline int Log2Int(uint32_t v)
{
#if defined(PBRT_IS_MSVC)
	unsigned long lz = 0;
	if (_BitScanReverse(&lz, v)) return lz;
	return 0;
#else
	return 31 - __builtin_clz(v);
#endif
}

inline int Log2Int(int32_t v) { return Log2Int((uint32_t)v); }

inline int Log2Int(uint64_t v)
{
#if defined(PBRT_IS_MSVC)
	unsigned long lz = 0;
#if defined(_WIN64)
	_BitScanReverse64(&lz, v);
#else
	if (_BitScanReverse(&lz, v >> 32))
		lz += 32;
	else
		_BitScanReverse(&lz, v & 0xffffffff);
#endif // _WIN64
	return lz;
#else  // PBRT_IS_MSVC
	return 63 - __builtin_clzll(v);
#endif
}

inline int Log2Int(int64_t v) { return Log2Int((uint64_t)v); }

template <typename T> inline bool IsPowerOf2(T v)
{
	return v && !(v & (v - 1));
}

inline int32_t RoundUpPow2(int32_t v)
{
	v--;
	v |= v >> 1;    v |= v >> 2;
	v |= v >> 4;    v |= v >> 8;
	v |= v >> 16;
	return v + 1;
}

inline int64_t RoundUpPow2(int64_t v)
{
	v--;
	v |= v >> 1;    v |= v >> 2;
	v |= v >> 4;    v |= v >> 8;
	v |= v >> 16;   v |= v >> 32;
	return v + 1;
}

template <typename Predicate> int FindInterval(int size, const Predicate& pred)
{
	int first = 0, len = size;
	while (len > 0)
	{
		int half = len >> 1, middle = first + half;
		// Bisect range based on value of _pred_ at _middle_
		if (pred(middle))
		{
			first = middle + 1;
			len -= half + 1;
		}
		else
			len = half;
	}
	return Clamp(first - 1, 0, size - 2);
}

#endif  // PBRT_CORE_PBRT_H