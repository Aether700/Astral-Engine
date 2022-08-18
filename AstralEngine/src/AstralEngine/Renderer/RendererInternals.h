#pragma once
#include "AstralEngine/Math/AMath.h"
#include "Renderer.h"


namespace AstralEngine
{
	enum class RenderableType
	{
		AEntity, // renderable comes from an entity of the scene
		Quad,
	};

	class DrawCommand
	{
	public:
		DrawCommand();
		DrawCommand(const Mat4& transform, AReference<Material> mat, RenderableType type);

		const Mat4& GetTransform() const;
		const AReference<Material>& GetMaterial() const;
		RenderableType GetType() const;
		bool IsOpaque() const;

		bool operator==(const DrawCommand& other) const;
		bool operator!=(const DrawCommand& other) const;

	private:
		Mat4 m_transform;
		RenderableType m_type;
		AReference<Material> m_material;
	};

	// ordered list of DrawCommands. Used to sort in what order what should be drawn
	class DrawList
	{
	public:
		~DrawList();

		think over how draw cmds are stored internally and then implement draw list class

		void AddDrawCmd(DrawCommand* cmd);
		DrawCommand* GetNextDrawCmd();
		bool IsEmpty() const;

	private:
		ADoublyLinkedList<DrawCommand*> m_drawCommands;
	};
}