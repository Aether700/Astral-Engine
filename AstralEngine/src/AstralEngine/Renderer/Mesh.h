#pragma once
#include "AstralEngine/Data Struct/ADynArr.h"
#include "AstralEngine/Data Struct/ADataStorage.h"
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Core/Resource.h"

#include <string>
#include <fstream>

namespace AstralEngine
{
	// stores the reprensentation of the triangles in a mesh in such a way that information about the individual 
	// triangles, points and edges can easily be extracted. Also supports adding and removing triangles from the mesh
	class MeshDataManipulator
	{
	public:
		// returns the id of the triangle or NullID if the triangle is invalid (example has 2 points equal)
		size_t AddTriangle(Vector3 p1, Vector3 p2, Vector3 p3);
		size_t AddTriangle(size_t p1Index, size_t p2Index, size_t p3Index);
		
		void RemoveTriangle(size_t triangle);

		// returns the id of the i-th edge of the triangle
		size_t GetEdge1ID(size_t triangleID) const;
		size_t GetEdge2ID(size_t triangleID) const;
		size_t GetEdge3ID(size_t triangleID) const;

		// returns the id of the i-th point of the edge
		size_t GetPoint1ID(size_t edgeID) const;
		size_t GetPoint2ID(size_t edgeID) const;

		// returns the id of the i-th point of the triangle
		size_t GetTrianglePoint1ID(size_t triangle) const;
		size_t GetTrianglePoint2ID(size_t triangle) const;
		size_t GetTrianglePoint3ID(size_t triangle) const;
		const Vector3& GetTriangleCicumsphereCenter(size_t triangle) const;

		const Vector3& GetCoords(size_t pointID) const;

		bool TrianglesAreAdjacent(size_t t1, size_t t2) const;
		bool TrianglesSharePoint(size_t t1, size_t t2) const;
		bool EdgesSharePoint(size_t e1, size_t e2) const;
		bool PointIsInTriangleCircumsphere(size_t triangle, const Vector3& point) const;
		bool PointIsInTriangleCircumsphere(size_t triangle, size_t point) const;
		
		bool EdgeContainsPoint(size_t edge, size_t point) const;
		bool TriangleContainsEdge(size_t triangle, size_t edge) const;
		bool TriangleContainsPoint(size_t triangle, size_t point) const;

		bool TriangleIDIsValid(size_t id) const;
		bool EdgeIDIsValid(size_t id) const;
		bool PointIDIsValid(size_t id) const;

	private:
		size_t CreatePoint(const Vector3& point);
		size_t CreateEdge(size_t p1, size_t p2);
		size_t CreateTriangle(size_t e1, size_t e2, size_t e3);
		void ComputeCircumsphereOfTriangle(size_t triangle);

		void RemoveTriangleFromEdge(size_t triangle, size_t edge);
		void RemoveEdgeFromPoint(size_t edge, size_t point);

		struct Point
		{
			Vector3 coords;
			ASinglyLinkedList<size_t> edgesSharingThisPoint;

			bool operator==(const Point& other) const
			{
				return coords == other.coords;
			}

			bool operator!=(const Point& other) const
			{
				return !(*this == other);
			}
		};

		struct Edge
		{
			size_t point1;
			size_t point2;

			ASinglyLinkedList<size_t> trianglesSharingThisEdge;

			bool operator==(const Edge& other) const
			{
				return (point1 == other.point1 && point2 == other.point2)
					|| (point1 == other.point2 && point2 == other.point1);
			}

			bool operator!=(const Edge& other) const
			{
				return !(*this == other);
			}
		};

		struct Triangle
		{
			size_t edge1;
			size_t edge2;
			size_t edge3;
			Vector3 circumsphereCenter;
			float circumsphereRadiusSqr;

			bool operator==(const Triangle& other) const
			{
				return (edge1 == other.edge1 && edge2 == other.edge2 && edge3 == other.edge3)
					|| (edge1 == other.edge1 && edge2 == other.edge3 && edge3 == other.edge2)
					|| (edge1 == other.edge2 && edge2 == other.edge1 && edge3 == other.edge3)
					|| (edge1 == other.edge2 && edge2 == other.edge3 && edge3 == other.edge1)
					|| (edge1 == other.edge3 && edge2 == other.edge1 && edge3 == other.edge2)
					|| (edge1 == other.edge3 && edge2 == other.edge2 && edge3 == other.edge1);
			}

			bool operator!=(const Triangle& other) const
			{
				return !(*this == other);
			}
		};

		ADataStorage<Point> m_points;
		ADataStorage<Edge> m_edges;
		ADataStorage<Triangle> m_triangles;
	};

	/* represents the mesh obtained when loading a model from a file
	*/
	class Mesh
	{
		friend class ResourceHandler;
	public:
		Mesh(const ADynArr<Vector3>& positions, const ADynArr<Vector2>& textureCoords,
			const ADynArr<Vector3>& normals, const ADynArr<unsigned int>& indices);

		const ADynArr<Vector3>& GetPositions() const;
		const ADynArr<Vector2>& GetTextureCoords() const;
		const ADynArr<Vector3>& GetNormals() const;
		const ADynArr<unsigned int>& GetIndices() const;

		static MeshHandle QuadMesh();

	private:
		static MeshHandle GenerateQuadMesh();

		//loads a model from a file and creates a Mesh object. returns nullptr if an error occurs
		static AReference<Mesh> LoadFromFile(const std::string& filepath);

		static std::string GetFileExtension(const std::string& filepath);
		static AReference<Mesh> LoadFromOBJ(const std::string& filepath);

		//helper function which tries to find the VertexData and returns it's index or returns -1 if it couldn't be found
		static int FindVertexData(const ADynArr<Vector3>& outPositions, const ADynArr<Vector2>& outTextureCoords,
			const ADynArr<Vector3>& outNormals, const Vector3& pos, const Vector2& textureCoords, 
			const Vector3& normal);

		ADynArr<Vector3> m_positions;
		ADynArr<Vector2> m_textureCoords;
		ADynArr<Vector3> m_normals;
		ADynArr<unsigned int> m_indices;
	};
}