#pragma once
#include "Layer.h"
#include "AstralEngine/Data Struct/ADynArr.h"

namespace AstralEngine
{
	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		inline int GetCount() const { return m_layers.GetCount(); }

		void AttachLayer(Layer* l);
		void DetachLayer(Layer* l);
		void AttachOverlay(Layer* l);
		void DetachOverlay(Layer* l);

		Layer* Get(unsigned int index) { return operator[](index); }

		Layer* operator[](unsigned int index);

	private:
		ADynArr<Layer*> m_layers;
		int m_insertIndex;
	};
}