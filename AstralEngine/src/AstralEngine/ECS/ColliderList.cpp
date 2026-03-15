#include "aepch.h"
#include "ColliderList.h"

namespace AstralEngine
{
	// ColliderList ////////////////////////////////////////////////////////////////////
	ColliderList::ColliderList() { }
	ColliderList::ColliderList(const ColliderList&) { }
	ColliderList::ColliderList(ColliderList&& other) noexcept : ComponentPairListBase(std::move(other)) { }

	ColliderList& ColliderList::operator=(const ColliderList& other)
	{
		return (ColliderList&)AssignmentOperator(other);
	}

	ColliderList& ColliderList::operator=(ColliderList&& other) noexcept
	{
		return (ColliderList&)MoveOperator(std::move(other));
	}

	bool ColliderList::operator==(const ColliderList& other) const
	{
		return EqualOperator(other);
	}

	bool ColliderList::operator!=(const ColliderList& other) const
	{
		return !(*this == other);
	}
}