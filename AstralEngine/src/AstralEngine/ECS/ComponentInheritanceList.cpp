#include "aepch.h"
#include "ComponentInheritanceLists.h"
#include "AEntity.h"

namespace AstralEngine
{
	// ComponentPairInstanceBase /////////////////////////////////////////////////////////////////
	ComponentPairInstanceBase::ComponentPairInstanceBase(BaseEntity entityID, Scene* scene) 
		: m_scene(scene), m_entityID(entityID) { }

	AEntity ComponentPairInstanceBase::GetAEntity() { return AEntity(m_entityID, m_scene); }
}