#include "aepch.h"
#include "Mesh.h"

namespace AstralEngine
{
	// MeshDataView ////////////////////////////////////////////////////////////////////////////////////
	size_t MeshDataView::AddTriangle(const Vector3& p1, const Vector3& p2, const Vector3& p3)
	{
		return AddTriangle(CreatePoint(p1), CreatePoint(p2), CreatePoint(p3));
	}

	size_t MeshDataView::AddTriangle(size_t p1Index, size_t p2Index, size_t p3Index)
	{
		size_t e1 = CreateEdge(p1Index, p2Index);
		size_t e2 = CreateEdge(p1Index, p3Index);
		size_t e3 = CreateEdge(p2Index, p3Index);

		return CreateTriangle(e1, e2, e3);
	}

	bool MeshDataView::TrianglesAreAdjacent(size_t t1, size_t t2) const
	{
		const Triangle& triangle1 = m_triangles[t1];
		const Triangle& triangle2 = m_triangles[t2];

		return triangle1.edge1 == triangle2.edge1 || triangle1.edge1 == triangle2.edge2 
			|| triangle1.edge1 == triangle2.edge3 || triangle1.edge2 == triangle2.edge1
			|| triangle1.edge2 == triangle2.edge2 || triangle1.edge2 == triangle2.edge3
			|| triangle1.edge3 == triangle2.edge1 || triangle1.edge3 == triangle2.edge2
			|| triangle1.edge3 == triangle2.edge3;
	}

	bool MeshDataView::PointIsInTriangleCircumsphere(size_t triangle, const Vector3& point) const
	{
		const Triangle& t = m_triangles[triangle];
		return Vector3::SqrDistance(t.circumsphereCenter, point) <= t.circumsphereRadiusSqr;
	}

	bool MeshDataView::PointIsInTriangleCircumsphere(size_t triangle, size_t point) const
	{
		return PointIsInTriangleCircumsphere(triangle, m_points[point]);
	}

	size_t MeshDataView::CreatePoint(const Vector3& point)
	{
		size_t id = m_points.FindID(point);

		if (id == NullID)
		{
			return m_points.Add(point);
		}
		return id;
	}

	size_t MeshDataView::CreateEdge(size_t p1, size_t p2)
	{
		Edge e;
		e.point1 = p1;
		e.point1 = p2;

		size_t id = m_edges.FindID(e);

		if (id == NullID)
		{
			return m_edges.Add(e);
		}
		return id;
	}
	
	size_t MeshDataView::CreateTriangle(size_t e1, size_t e2, size_t e3)
	{
		Triangle t;
		t.edge1 = e1;
		t.edge2 = e2;
		t.edge3 = e3;

		size_t id = m_triangles.FindID(t);

		if (id == NullID)
		{
			id = m_triangles.Add(t);
			ComputeCircumsphereOfTriangle(id);
			return id;
		}
		return id;
	}

	void MeshDataView::ComputeCircumsphereOfTriangle(size_t triangle)
	{
		Triangle& t = m_triangles[triangle];
		size_t pointA = m_edges[t.edge1].point1;
		size_t pointB = m_edges[t.edge1].point2;
		size_t pointC;
		
		{
			Edge e2 = m_edges[t.edge2];
			if (e2.point1 == pointA || e2.point1 == pointB)
			{
				pointC = e2.point2;
			}
			else
			{
				pointC = e2.point1;
			}
		}

		Vector3 ac = m_points[pointC] - m_points[pointA];
		Vector3 ab = m_points[pointB] - m_points[pointA];
		Vector3 abCrossAc = Vector3::CrossProduct(ab, ac);
		
		Vector3 centerMinusA = (Vector3::CrossProduct(abCrossAc, ab) * ac.SqrMagnitude()
			+ Vector3::CrossProduct(ac, abCrossAc) * ab.SqrMagnitude()) / (2.0f * abCrossAc.SqrMagnitude());
		t.circumsphereRadiusSqr = centerMinusA.SqrMagnitude();
		t.circumsphereCenter = centerMinusA + m_points[pointA];
	}

	// Mesh ///////////////////////////////////////////////////////////////////////////////////////
	Mesh::Mesh(const ADynArr<Vector3>& positions, const ADynArr<Vector2>& textureCoords,
		const ADynArr<Vector3>& normals, const ADynArr<unsigned int>& indices)
		: m_positions(positions), m_normals(normals), m_indices(indices), m_textureCoords(textureCoords)
	{ }

	const ADynArr<Vector3>& Mesh::GetPositions() const { return m_positions; }
	const ADynArr<Vector2>& Mesh::GetTextureCoords() const { return m_textureCoords; }
	const ADynArr<Vector3>& Mesh::GetNormals() const { return m_normals; }
	const ADynArr<unsigned int>& Mesh::GetIndices() const { return m_indices; }

	MeshHandle Mesh::QuadMesh()
	{
		static MeshHandle quadMesh = GenerateQuadMesh();
		return quadMesh;
	}

	MeshHandle Mesh::GenerateQuadMesh()
	{
		ADynArr<Vector3> positions = {
			{ -0.5f, -0.5f, 0.0f },
			{  0.5f, -0.5f, 0.0f },
			{  0.5f,  0.5f, 0.0f },
			{ -0.5f,  0.5f, 0.0f }
		};

		ADynArr<Vector3> normals = {
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 1.0f }
		};

		ADynArr<Vector2> textureCoords = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		ADynArr<unsigned int> indices = {
			0, 1, 2,
			2, 3, 0
		};

		return ResourceHandler::CreateMesh(positions, textureCoords, normals, indices);
	}

	//loads a model from a file and creates a Mesh object. returns nullptr if an error occurs
	AReference<Mesh> Mesh::LoadFromFile(const std::string& filepath)
	{
		std::string extension = GetFileExtension(filepath);

		if (extension == "obj")
		{
			return LoadFromOBJ(filepath);
		}

		AE_CORE_ERROR("file format of \"%S\" not supported\n", filepath);
		return nullptr;
	}

	std::string Mesh::GetFileExtension(const std::string& filepath)
	{
		size_t lastDot = filepath.rfind('.');

		//no extension
		if (lastDot == std::string::npos)
		{
			return "";
		}

		return filepath.substr(lastDot + 1, filepath.size() - lastDot + 1);
	}

	AReference<Mesh> Mesh::LoadFromOBJ(const std::string& filepath)
	{
		std::ifstream file(filepath);

		if (!file)
		{
			AE_CORE_WARN("could not load file \"%s\"", filepath);
			return nullptr;
		}

		ADynArr<Vector3> positions;
		ADynArr<Vector2> textureCoords;
		ADynArr<Vector3> normals;

		ADynArr<unsigned int> posIndices;
		//index of the face first then the actual value
		ADynArr<std::pair<unsigned int, unsigned int>> texCoordIndices;
		ADynArr<std::pair<unsigned int, unsigned int>> normalIndices;

		unsigned int vertexIndex = 0;

		while (true)
		{
			if (file.eof())
			{
				break;
			}

			char buffer[200];
			file.getline(buffer, 200);//read line


			if (buffer[0] == 'v' && buffer[1] == ' ') //defining a postion
			{
				Vector3 position = Vector3(0, 0, 0);
				int result = sscanf_s(buffer, "v %f %f %f\n", &position.x, &position.y, &position.z);

				if (result != 3)
				{
					AE_WARN("Unexpected internal format when reading vertex position in file %s", filepath);
					return nullptr;
				}
				positions.Add(position);
			}
			else if (buffer[0] == 'v' && buffer[1] == 't' && buffer[2] == ' ') //defining a texture coordinate
			{
				Vector3 texCoord = Vector3(0, 0, 0);
				int result = sscanf_s(buffer, "vt %f %f\n", &texCoord.x, &texCoord.y);

				if (result != 2)
				{
					result = sscanf_s(buffer, "vt %f %f %f\n", &texCoord.x, &texCoord.y, &texCoord.z);
					if (result != 3)
					{
						AE_WARN("Unexpected internal format when reading vertex texture coordinates"
							"in file %s", filepath);
						return nullptr;
					}
				}
				textureCoords.Add(texCoord);
			}
			else if (buffer[0] == 'v' && buffer[1] == 'n' && buffer[2] == ' ') //defining a normal
			{
				Vector3 normal = Vector3(0, 0, 0);
				int result = sscanf_s(buffer, "vn %f %f %f\n", &normal.x, &normal.y, &normal.z);

				if (result != 3)
				{
					AE_WARN("Unexpected internal format when reading vertex normal in file %s", filepath);
					return nullptr;
				}
				normals.Add(normal);
			}
			else if (buffer[0] == 'f' && buffer[1] == ' ') //defining a face
			{
				int posIndex[3];
				int textureCoordsIndex[3];
				int normalIndex[3];

				bool hasTexCoords = true;
				bool hasNormals = true;

				//try format pos/tex/normal
				int result = sscanf_s(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &posIndex[0], &textureCoordsIndex[0],
					&normalIndex[0], &posIndex[1], &textureCoordsIndex[1], &normalIndex[1], &posIndex[2],
					&textureCoordsIndex[2], &normalIndex[2]);

				if (result != 9)
				{
					//try format pos//normal
					result = sscanf_s(buffer, "f %d//%d %d//%d %d//%d\n", &posIndex[0], &normalIndex[0], &posIndex[1],
						&normalIndex[1], &posIndex[2], &normalIndex[2]);

					if (result != 6)
					{
						//try format pos/tex
						result = sscanf_s(buffer, "f %d/%d %d/%d %d/%d\n", &posIndex[0], &textureCoordsIndex[0],
							&posIndex[1], &textureCoordsIndex[1], &posIndex[2], &textureCoordsIndex[2]);

						if (result != 6)
						{
							//try format pos//
							result = sscanf_s(buffer, "f %d// %d// %d//\n", &posIndex[0], &posIndex[1], &posIndex[2]);

							if (result != 3)
							{
								AE_WARN("Unknown face format detected in file %s", filepath);
								return nullptr;
							}

							hasTexCoords = false;
						}

						hasNormals = false;
					}
					else
					{
						hasTexCoords = false;
					}
				}

				posIndices.Add(posIndex[0] - 1);
				posIndices.Add(posIndex[1] - 1);
				posIndices.Add(posIndex[2] - 1);

				if (hasNormals)
				{
					normalIndices.Add(std::make_pair(vertexIndex, (unsigned int)normalIndex[0] - 1U));
					normalIndices.Add(std::make_pair(vertexIndex + 1, (unsigned int)normalIndex[1] - 1U));
					normalIndices.Add(std::make_pair(vertexIndex + 2, (unsigned int)normalIndex[2] - 1U));
				}

				if (hasTexCoords)
				{
					texCoordIndices.Add(std::make_pair(vertexIndex, (unsigned int)textureCoordsIndex[0] - 1U));
					texCoordIndices.Add(std::make_pair(vertexIndex + 1, (unsigned int)textureCoordsIndex[1] - 1U));
					texCoordIndices.Add(std::make_pair(vertexIndex + 2, (unsigned int)textureCoordsIndex[2] - 1U));
				}

				vertexIndex += 3;
			}
		}

		ADynArr<Vector3> outPositions;
		ADynArr<Vector2> outTextureCoords;
		ADynArr<Vector3> outNormals;
		ADynArr<unsigned int> outIndices;

		int textureIndex = 0;
		int normalIndex = 0;

		int index = 0;
		for (unsigned int i = 0; i < posIndices.GetCount(); i++)
		{
			Vector3 texCoords;
			if (textureCoords.GetCount() != 0 && texCoordIndices[textureIndex].first == i)
			{
				texCoords = textureCoords[texCoordIndices[textureIndex].second];
				textureIndex++;
			}

			Vector3 normal;
			if (normals.GetCount() != 0 && normalIndices[normalIndex].first == i)
			{
				normal = normals[normalIndices[normalIndex].second];
				normalIndex++;
			}

			int currIndex = FindVertexData(outPositions, outTextureCoords, outNormals,
				positions[posIndices[i]], texCoords, normal);

			if (currIndex == -1)
			{
				currIndex = index;
				index++;

				//add a vertex point
				outPositions.Add(positions[posIndices[i]]);
				outTextureCoords.Add(texCoords);
				outNormals.Add(normal);
			}

			outIndices.Add((unsigned int)currIndex);
		}

		return AReference<Mesh>::Create(outPositions, outTextureCoords, outNormals, outIndices);
	}

	//helper function which tries to find the VertexData and returns it's index or returns -1 
	//if it couldn't be found
	int Mesh::FindVertexData(const ADynArr<Vector3>& outPositions, const ADynArr<Vector2>& outTextureCoords,
		const ADynArr<Vector3>& outNormals, const Vector3& pos, const Vector2& textureCoords, const Vector3& normal)
	{
		for (unsigned int i = 0; i < outPositions.GetCount(); i++)
		{
			if (outPositions[i] == pos && outTextureCoords[i] == textureCoords && outNormals[i] == normal)
			{
				return i;
			}
		}
		return -1;
	}
}