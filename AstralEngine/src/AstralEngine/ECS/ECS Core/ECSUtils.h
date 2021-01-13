#pragma once
#include <type_traits>

namespace AstralEngine
{
	using IDType = size_t;

	enum class BaseEntity : IDType { };

	constexpr auto ToIntegral(const BaseEntity entity)
	{
		return static_cast<std::underlying_type_t<BaseEntity>>(entity);
	}

	//Null Entity object
	class NullObj
	{
	public:
		template<typename Entity>
		constexpr operator Entity() const
		{
			return Entity{ (size_t)((size_t)0 - (size_t)1) }; //get MaxValue of the unsigned IDType
		}

		constexpr bool operator==(NullObj) const { return true; }
		constexpr bool operator!=(NullObj) const { return false; }

		template<typename Entity>
		constexpr bool operator==(const Entity& e) const
		{
			return ToIntegral(e) == ToIntegral(static_cast<Entity>(*this));
		}

		template<typename Entity>
		constexpr bool operator!=(const Entity& e) const 
		{
			return !(e == *this);
		}
	};

	template<typename Entity>
	constexpr bool operator==(const Entity& e, NullObj n)
	{
		return ToIntegral(static_cast<Entity>(n)) == ToIntegral(e);
	}

	template<typename Entity>
	constexpr bool operator!=(const Entity& e, NullObj n)
	{
		return !(e == n);
	}

	inline constexpr auto Null = NullObj{};

	/*basic struct which acts as a list of types

	  we can access the different types by doing "Type..." to "loop" through the different types
	*/
	template<typename...>
	struct TypeList {};

	//variable used to increase readability
	template<typename... Type>
	const TypeList<Type...> get = {};

	//variable used to increase readability
	template<typename... Type>
	const TypeList<Type...> exclude = {};

	/*Concatenation of different types into a single list
	  not defined on purpose to force use of specializations
	*/
	template<typename...>
	struct TypeListCat;

	//Simple List of concatenated Types
	template<>
	struct TypeListCat<>
	{
		using Type = TypeList<>;
	};

	/* specialization which takes 2 Types and a list of types
	*/
	template <typename... Type1, typename... Type2, typename... List>
	struct TypeListCat<TypeList<Type1...>, TypeList<Type2...>, List...>
	{
		using Type = typename TypeListCat<TypeList<Type1..., Type2...>, List...>;
	};

	template<typename... T>
	struct TypeListCat<TypeList<T...>>
	{
		using Type = TypeList<T...>;
	};

	//global static index counter
	struct TypeIndex
	{
		static unsigned int Next()
		{
			static unsigned int index = 0;
			return index++;
		}
	};
	
	//Provides an index per Component Type
	template <typename Component>
	struct IndexProvider
	{
		static unsigned int GetIndex()
		{
			static unsigned index = TypeIndex::Next();
			return index;
		}
	};

	template<typename Type>
	struct TypeInfo
	{
		static unsigned int ID()
		{
			return IndexProvider<Type>::GetIndex();
		}
	};

	//default value of the HasIndex struct, will default to this in case of failing type "comparaisons"
	template<typename, typename = void>
	struct HasIndex : std::false_type {};

	/*type tries to comform to this definition, if it fails it will conform to the default one above
	  std::void_t tests the expression provided (here "decltype(IndexProvider<Type>::GetIndex())>")
	  where decltype tries to return the type of IndexProvider<Type> 
	  if it exists (std::void_t will fail the expression if it does not)
	*/
	template<typename Type>
	struct HasIndex<Type, std::void_t<decltype(IndexProvider<Type>::GetIndex())>> : std::true_type {};

}	