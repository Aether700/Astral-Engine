#pragma once
#include "ComponentInheritanceLists.h"

namespace AstralEngine
{
	class Collider2D;

	// base object used to gather pairs of AEntity objects and different types 
	//of Collider2D objects
	class Collider2DAEntityPair : ComponentPairBase
	{
	public:
		virtual ~Collider2DAEntityPair() { }
		virtual void OnCreate() = 0;
		virtual void OnDestroy() = 0;
		virtual Collider2D& GetCollider() = 0;
	};

	template<typename Component>
	class Collider2DInstanceAEntityPair : public Collider2DAEntityPair, public ComponentPairInstanceBase
	{
	public:
		Collider2DInstanceAEntityPair(BaseEntity e, Scene* s) : ComponentPairInstanceBase(e, s) { }

		virtual void OnCreate() override
		{
			GetAEntity().GetComponent<Component>().OnCreate();
		}

		virtual void OnDestroy() override 
		{
			GetAEntity().GetComponent<Component>().OnDestroy();
		}


		virtual Collider2D& GetCollider() override
		{
			return GetAEntity().GetComponent<Component>();
		}

		virtual size_t GetTypeID() const override
		{
			return TypeInfo<Component>::ID();
		}
	};

	// add all callback components to this list so they can easily be retrieved 
	// and their callbacks can be accessed easily
	class ColliderList : ComponentPairListBase<Collider2DAEntityPair>
	{
	public:
		ColliderList();
		ColliderList(const ColliderList&);
		ColliderList(ColliderList&& other) noexcept;


		template<typename Component>
		void Add(BaseEntity e, Scene* s)
		{
			ComponentPairListBase::Add<Collider2DInstanceAEntityPair<Component>>(e, s)->OnCreate();
		}

		template<typename Component>
		void Remove()
		{
			((Collider2DInstanceAEntityPair<Component>*)ComponentPairListBase::Remove<Component>(false))
				->OnDestroy();
		}

		ColliderList& operator=(const ColliderList& other);
		ColliderList& operator=(ColliderList&& other) noexcept;
		bool operator==(const ColliderList& other) const;
		bool operator!=(const ColliderList& other) const;
	};
}