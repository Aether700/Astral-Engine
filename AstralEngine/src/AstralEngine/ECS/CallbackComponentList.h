#pragma once
#include "ComponentInheritanceLists.h"

namespace AstralEngine
{
	// base object used to gather pairs of AEntity objects and different types 
	//of CallbackComponent objects
	class CallbackAEntityPair : ComponentPairBase
	{
	public:
		virtual ~CallbackAEntityPair() { }
		virtual void OnCreate() = 0;
		virtual void OnDestroy() = 0;
		virtual void OnStart() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnLateUpdate() = 0;
	};

	template<typename Component>
	class ComponentAEntityPair : public CallbackAEntityPair, public ComponentPairInstanceBase
	{
	public:
		ComponentAEntityPair(BaseEntity e, Scene* s) : ComponentPairInstanceBase(e, s) { }

		virtual void OnCreate() override
		{
			GetAEntity().GetComponent<Component>().OnCreate();
		}

		virtual void OnDestroy() override
		{
			GetAEntity().GetComponent<Component>().OnDestroy();
		}

		virtual void OnStart() override
		{
			if (AEntityAndComponentAreActive())
			{
				GetAEntity().GetComponent<Component>().OnStart();
			}
		}

		virtual void OnUpdate() override
		{
			if (AEntityAndComponentAreActive())
			{
				GetAEntity().GetComponent<Component>().OnUpdate();
			}
		}

		virtual void OnLateUpdate() override
		{
			if (AEntityAndComponentAreActive())
			{
				GetAEntity().GetComponent<Component>().OnLateUpdate();
			}
		}

		virtual size_t GetTypeID() const override
		{
			return TypeInfo<Component>::ID();
		}

	private:
		bool AEntityAndComponentAreActive()
		{
			if (GetAEntity().IsActive())
			{
				Component& comp = GetAEntity().GetComponent<Component>();
				return comp.IsActive();
			}
			return false;
		}
	};

	// add all callback components to this list so they can easily be retrieved 
	// and their callbacks can be accessed easily
	class CallbackList : ComponentPairListBase<CallbackAEntityPair>
	{
	public:
		CallbackList();
		CallbackList(const CallbackList&);
		CallbackList(CallbackList&& other) noexcept;


		template<typename Component>
		void Add(BaseEntity e, Scene* s)
		{
			ComponentPairListBase::Add<ComponentAEntityPair<Component>>(e, s)->OnCreate();
		}

		template<typename Component>
		void Remove()
		{
			((ComponentAEntityPair<Component>*)ComponentPairListBase::Remove<Component>(false))->OnDestroy()
		}

		void CallOnStart();
		void CallOnUpdate();
		void CallOnLateUpdate();


		CallbackList& operator=(const CallbackList& other);
		CallbackList& operator=(CallbackList&& other) noexcept;
		bool operator==(const CallbackList& other) const;
		bool operator!=(const CallbackList& other) const;
	};

}