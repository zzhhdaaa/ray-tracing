#pragma once

#include <glm/glm.hpp>

struct BoundingBox
{
	BoundingBox(glm::vec3 a, glm::vec3 b) { MinPos = a; MaxPos = b; }
	glm::vec3 MinPos;
	glm::vec3 MaxPos;
};

//struct TriangleMesh
//{
//	// ObjectToWorld: The object - to - world transformation for the mesh.
//	// nTriangles : The total number of triangles in the mesh.
//	// vertexIndices : A pointer to an array of vertex indices.For the ith triangle, its three vertex positions are P[vertexIndices[3 * i]], P[vertexIndices[3 * i + 1]], and P[vertexIndices[3 * i + 2]].
//	// nVertices : The total number of vertices in the mesh.
//	// P : An array of nVertices vertex positions.
//	// S : An optional array of tangent vectors, one per vertex in the mesh.These are used to compute shading tangents.
//	// N : An optional array of normal vectors, one per vertex in the mesh.If present, these are interpolated across triangle faces to compute shading normals.
//	// UV : An optional array of parametric (u, v) values, one for each vertex.
//	// alphaMask : An optional alpha mask texture, which can be used to cut away parts of triangle surfaces.
//
//	// TriangleMesh Public Methods
//	// TriangleMesh Data
//	const int nTriangles, nVertices;
//	std::vector<int> vertexIndices;
//	std::unique_ptr<glm::vec3[]> p;
//	std::unique_ptr<glm::vec3[]> n;
//	std::unique_ptr<glm::vec3[]> s;
//	std::unique_ptr<glm::vec2[]> uv;
//	//std::shared_ptr<Texture<Float>> alphaMask;
//};

class Geometry
{
public:
	int MaterialIndex = 0;
	//virtual bool GetBoundingBox(BoundingBox& outputBox) const = 0;
};

class Sphere : public Geometry
{
public:
	Sphere() : Origin(glm::vec3(0.0f)), Radius(0.5f) {}
	Sphere(glm::vec3 origin, float radius) : Origin(origin), Radius(radius) {}
	glm::vec3 Origin;
	float Radius;

	//virtual bool GetBoundingBox(BoundingBox& outputBox) const override;
};

//class Triangle : public Geometry
//{
//public:
//private:
//	// Triangle Private Data
//	std::shared_ptr<TriangleMesh> mesh;
//	const int* v;
//};

//class GeometryList : public Geometry
//{
//	virtual bool GetBoundingBox(BoundingBox& outputBox) const override;
//};