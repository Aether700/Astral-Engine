#pragma once
#include "AstralEngine/Data Struct/ADynArr.h"
#include "AstralEngine/Math/AMath.h"

#include <string>
#include <fstream>

namespace AstralEngine
{
	/* represents the mesh obtained when loading a model from a file
	*/
	class Mesh
	{
	public:
		Mesh(const ADynArr<Vector3>& positions, const ADynArr<Vector3>& textureCoords,
			const ADynArr<Vector3>& normals, const ADynArr<unsigned int>& indices)
			: m_positions(positions), m_normals(normals), m_indices(indices), m_textureCoords(textureCoords)
		{ }

		Mesh(const ADynArr<Vector3>& positions, const ADynArr<Vector2>& textureCoords,
			const ADynArr<Vector3>& normals, const ADynArr<unsigned int>& indices)
			: m_positions(positions), m_normals(normals), m_indices(indices), m_textureCoords(textureCoords.GetCount())
		{
			for (const Vector2& coords : textureCoords)
			{
				m_textureCoords.Add(Vector3(coords.x, coords.y, 0));
			}
		}

		const ADynArr<Vector3>& GetPositions() const { return m_positions; }
		const ADynArr<Vector3>& GetTextureCoords() const { return m_textureCoords; }
		const ADynArr<Vector3>& GetNormals() const { return m_normals; }
		const ADynArr<unsigned int>& GetIndices() const { return m_indices; }

		//loads a model from a file and creates a Mesh object. returns nullptr if an error occurs
		static std::shared_ptr<Mesh> LoadFromFile(const std::string& filepath)
		{
			std::string extension = GetFileExtension(filepath);

			if (extension == "obj")
			{
				return LoadFromOBJ(filepath);
			}

			std::cout << "file format of \"" << filepath << "\" not supported\n";
			return nullptr;
		}

	private:
		static std::string GetFileExtension(const std::string& filepath)
		{
			int lastDot = filepath.rfind('.');

			//no extension
			if (lastDot == std::string::npos)
			{
				return "";
			}

			return filepath.substr(lastDot + 1, filepath.size() - lastDot + 1);
		}

		static std::shared_ptr<Mesh> LoadFromOBJ(const std::string& filepath)
		{
			std::ifstream file(filepath);

			if (!file)
			{
				AE_CORE_WARN("could not load file \"%s\"", filepath);
				return nullptr;
			}

			ADynArr<Vector3> positions;
			ADynArr<Vector3> textureCoords;
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
			ADynArr<Vector3> outTextureCoords;
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

			return std::make_shared<Mesh>(outPositions, outTextureCoords, outNormals, outIndices);
		}

		//helper function which tries to find the VertexData and returns it's index or returns -1 if it couldn't be found
		static int FindVertexData(const ADynArr<Vector3>& outPositions,
			const ADynArr<Vector3>& outTextureCoords,
			const ADynArr<Vector3>& outNormals, const Vector3& pos, const Vector3& textureCoords, const Vector3& normal)
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

		ADynArr<Vector3> m_positions;
		ADynArr<Vector3> m_textureCoords;
		ADynArr<Vector3> m_normals;
		ADynArr<unsigned int> m_indices;
	};
}