#pragma once
#include "AstralEngine/Data Struct/ADynArr.h"
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Core/Resource.h"

#include <string>
#include <fstream>

namespace AstralEngine
{
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