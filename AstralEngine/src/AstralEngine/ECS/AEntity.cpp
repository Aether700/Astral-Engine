#include "aepch.h"
#include "AEntity.h"
#include "Components.h"

namespace AstralEngine
{
	const std::string& AEntity::GetName() const { return GetComponent<NameComponent>().name; }
	void AEntity::SetName(const std::string& newName) { GetComponent<NameComponent>().name = newName; }

	TransformComponent& AEntity::GetTransform() { return GetComponent<TransformComponent>(); }
	const TransformComponent& AEntity::GetTransform() const { return GetComponent<TransformComponent>(); }
}