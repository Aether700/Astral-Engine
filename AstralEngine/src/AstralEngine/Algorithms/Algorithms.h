#pragma once
#include <functional>
#include "AstralEngine/Data Struct/ADelegate.h"
#include "AstralEngine/Math/AMath.h"

namespace AstralEngine
{
	//acts as a namespace/grouping for all the Algorithms
	class Algorithm
	{
	public:
		template<typename T>
		struct IsPtr;
		
		template<typename T>
		struct IsPtr<T*> : std::true_type { };

		template<typename T>
		struct IsPtr : std::false_type { };

		//swaps indexes i and j inside of the container
		template<typename T>
		static void Swap(T* container, size_t i, size_t j)
		{
			if (i == j)
			{
				return;
			}

			T temp = container[i];
			container[i] = container[j];
			container[j] = temp;
		}

		//performs in-place quicksort on the container provided using the comparing function provided
		template<typename T>
		static void QuickSort(T* container, size_t startIndex, size_t endIndex, 
			ADelegate<bool(const T&, const T&)> compareFunc)
		{
			if (endIndex - startIndex <= 1)
			{
				return;
			}
			else if (endIndex - startIndex == 2)
			{
				if (compareFunc(container[endIndex - 1], container[startIndex]))
				{
					Swap(container, startIndex, endIndex - 1);
				}
				return;
			}

			size_t pivotIndex = (endIndex - startIndex) / 2 + startIndex;
			T& pivot = container[pivotIndex];
			size_t wall = startIndex;
			bool swapped = false;
			for (size_t i = startIndex; i < endIndex; i++) 
			{
				if (compareFunc(container[i], pivot))
				{
					Swap(container, i, wall);
					wall++;
					swapped = true;
				}
			}

			if (!swapped)
			{
				Swap(container, pivotIndex, wall);
			}

			QuickSort(container, startIndex, wall, compareFunc);

			if (wall == startIndex)
			{
				wall++;
			}
			
			QuickSort(container, wall, endIndex, compareFunc);
		}

		template<typename T, typename Func>
		static void QuickSort(T* container, size_t startIndex, size_t endIndex, Func compareFunc)
		{
			std::conditional_t<IsPtr<T>::value,
				ADelegate<bool(const T&, const T&)>, ADelegate<bool(const T, const T)>> func(compareFunc);
			QuickSort(container, startIndex, endIndex, func);
		}

		//performs in-place quicksort on the ADynArr provided using the comparing function provided
		template<typename T>
		static void QuickSort(ADynArr<T>& arr, size_t startIndex, size_t endIndex,
			std::conditional_t<IsPtr<T>::value, ADelegate<bool(const T&, const T&)>, ADelegate<bool(const T, const T)>> compareFunc)
		{
			QuickSort(arr.GetData(), startIndex, endIndex, compareFunc);
		}

		template<typename T, typename Func>
		static void QuickSort(ADynArr<T> arr, size_t startIndex, size_t endIndex, Func compareFunc)
		{
			QuickSort(arr.GetData() , startIndex, endIndex, compareFunc);
		}
	};
}