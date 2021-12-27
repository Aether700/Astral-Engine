#include "aepch.h"
#include "AEntity.h"
#include "Components.h"

namespace AstralEngine
{
	const std::string& AEntity::GetName() const { return GetComponent<AEntityData>().GetName(); }
	void AEntity::SetName(const std::string& newName) { GetComponent<AEntityData>().SetName(newName); }

	Transform& AEntity::GetTransform() { return GetComponent<Transform>(); }
	const Transform& AEntity::GetTransform() const { return GetComponent<Transform>(); }

	bool AEntity::IsActive() const	{ return GetComponent<AEntityData>().IsActive(); }
	void AEntity::SetActive(bool val) { GetComponent<AEntityData>().SetActive(val); }
}