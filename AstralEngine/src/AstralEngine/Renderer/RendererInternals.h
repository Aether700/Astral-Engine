#pragma once
#include "AstralEngine/Math/AMath.h"
#include "Renderer.h"


namespace AstralEngine
{
	enum class RenderableType
	{
		Mesh,
		Quad
	};

	class DrawCommand
	{
	public:
		DrawCommand();
		DrawCommand(const Mat4& transform, MaterialHandle mat, RenderableType type);

		const Mat4& GetTransform() const;
		MaterialHandle GetMaterial() const;
		RenderableType GetType() const;
		bool IsOpaque() const;

		bool operator==(const DrawCommand& other) const;
		bool operator!=(const DrawCommand& other) const;

	private:
		Mat4 m_transform;
		RenderableType m_type;
		MaterialHandle m_material;
	};

	// lists all the data to be drawn to the screen in one or more draw calls
	class DrawCallList
	{

		// Eventually will need to keep track of what matrix is where in the m_instanceArrBuffer 
		// and only change the ones which have been modified

		// might want to check that in another object not sure yet
	public:
		DrawCallList();
		DrawCallList(MaterialHandle material, RenderableType type);
		~DrawCallList();

		MaterialHandle GetMaterial() const;
		RenderableType GetType() const;

		void Draw(const Mat4& viewProj) const;

		void AddDrawCommand(DrawCommand* draw);
		void Clear();

	private:
		void SetupGeometryData();
		void SetupQuad();

		ASinglyLinkedList<DrawCommand*> m_data;
		MaterialHandle m_material;
		RenderableType m_type;

		AReference<VertexBuffer> m_geometryDataBuffer;
		AReference<VertexBuffer> m_instanceArrBuffer;
		AReference<IndexBuffer> m_indexBuffer;
	};

	class RenderingDataSorter
	{
	public:
		using AIterator = AUnorderedMap<MaterialHandle, DrawCallList>::AIterator;
		using AConstIterator = AUnorderedMap<MaterialHandle, DrawCallList>::AConstIterator;

		void AddData(DrawCommand* data);
		void Clear();

		AIterator begin();
		AIterator end();

		AConstIterator begin() const;
		AConstIterator end() const;

	private:
		AUnorderedMap<MaterialHandle, DrawCallList> m_drawCalls;
	};

	// ordered list of DrawCommands. Used to sort in what order what should be drawn
	class DrawList
	{
	public:
		~DrawList();

		//think over how draw cmds are stored/sorted internally and then implement draw list class
		//need to group similar meshes/vertex data together to use instance rendering

		void AddDrawCmd(DrawCommand* cmd);
		DrawCommand* GetNextDrawCmd();
		bool IsEmpty() const;

	private:
		//ADoublyLinkedList<DrawCommand*> m_drawCommands;
	};
}