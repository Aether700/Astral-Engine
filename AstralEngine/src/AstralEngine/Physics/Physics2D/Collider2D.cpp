#include "aepch.h"
#include "Collider2D.h"
#include "Rigidbody2D.h"


namespace AstralEngine
{
	// Collider2D /////////////////////////////////////////////
	void Collider2D::OnCreate()
	{
		AE_CORE_ASSERT(HasComponent<Rigidbody2D>(), "Collider does not have a Rigidbody2D");
		Rigidbody2D& rb = GetComponent<Rigidbody2D>();
		AE_CORE_ASSERT(rb.m_collider == nullptr, "Rigidbody has more than one collider");
		rb.m_collider = this;
	}
	
	void Collider2D::OnDestroy()
	{
		AE_CORE_ASSERT(HasComponent<Rigidbody2D>(), "Collider does not have a Rigidbody2D");
		Rigidbody2D& rb = GetComponent<Rigidbody2D>();
		AE_CORE_ASSERT(rb.m_collider != nullptr, "Rigidbody has no collider to remove");
		rb.m_collider = nullptr;
	}

	// BoxCollider2D /////////////////////////////////////////
	void BoxCollider2D::OnCreate()
	{
		Collider2D::OnCreate();
		if (HasComponent<SpriteRenderer>())
		{
			m_width = GetTransform().scale.x;
			m_height = GetTransform().scale.y;
		}
	}

	float BoxCollider2D::GetMomentOfInertia() const
	{
		AE_CORE_ASSERT(HasComponent<Rigidbody2D>(), "BoxCollider2D Has no Rigidbody2D attached");
		Rigidbody2D& rb = GetComponent<Rigidbody2D>();
		return rb.GetMass() * (m_width * m_width + m_height * m_height) / 12.0f;
	}
}