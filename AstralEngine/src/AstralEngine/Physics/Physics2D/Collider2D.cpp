#include "aepch.h"
#include "Collider2D.h"
#include "Rigidbody2D.h"


namespace AstralEngine
{
	// Collider2D /////////////////////////////////////////////	
	void Collider2D::OnDestroy()
	{
		if (HasComponent<Rigidbody2D>())
		{
			GetComponent<Rigidbody2D>().SetInertia(1.0f);
		}
	}

	// BoxCollider2D /////////////////////////////////////////
	void BoxCollider2D::OnCreate()
	{
		if (HasComponent<Rigidbody2D>())
		{
			Rigidbody2D& rb = GetComponent<Rigidbody2D>();
			rb.SetInertia(GetMomentOfInertia(rb.GetMass()));
		}

		if (HasComponent<SpriteRenderer>())
		{
			m_width = GetTransform().GetScale().x;
			m_height = GetTransform().GetScale().y;
		}
	}

	float BoxCollider2D::GetMomentOfInertia(float mass) const
	{
		return mass * (m_width * m_width + m_height * m_height) / 12.0f;
	}
}