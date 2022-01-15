#pragma once
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/ECS/Components.h"

namespace AstralEngine
{
	//Basic parent class of all collider 2D objects
	class Collider2D : public EntityLinkedComponent, public CallbackComponent
	{
	public:
		virtual ~Collider2D() { }

		void OnCreate() override;
		void OnDestroy() override;

		Vector2& GetOffset() { return m_offset; }
		const Vector2& GetOffset() const { return m_offset; }
		void SetOffset(const Vector2& offset) { m_offset = offset; }

		virtual float GetMomentOfInertia() const = 0;

	private:
		Vector2 m_offset = Vector2::Zero();
	};

	class BoxCollider2D : public Collider2D
	{
	public:
		void OnCreate() override;

		float GetWidth() const { return m_width; }
		void SetWidth(float width) { m_width = width; }

		float GetHeight() const { return m_height; }
		void SetHeight(float height) { m_height = height; }

		virtual float GetMomentOfInertia() const override;

	private:
		float m_width = 1.0f;
		float m_height = 1.0f;
	};
}