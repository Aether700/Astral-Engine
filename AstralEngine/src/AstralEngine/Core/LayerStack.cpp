#include "aepch.h"
#include "LayerStack.h"

namespace AstralEngine
{
	LayerStack::LayerStack() : m_insertIndex(0) 
	{
	}
	
	LayerStack::~LayerStack() 
	{
		for (Layer* l : m_layers)
		{
			delete l;
		}
	}

	void LayerStack::AttachLayer(Layer* l) 
	{
		m_layers.Insert(l, m_insertIndex);
		m_insertIndex++;
		l->OnAttach();
	}

	void LayerStack::DetachLayer(Layer* l) 
	{
		int index = m_layers.Find(l);
		if (index == -1)
		{
			return;
		}
		else if (index < m_insertIndex)
		{
			m_insertIndex--;
		}
		m_layers.RemoveAt(index);
		l->OnDetach();
	}

	void LayerStack::AttachOverlay(Layer* l) 
	{
		m_layers.AddLast(l);
		l->OnAttach();
	}
	
	void LayerStack::DetachOverlay(Layer* l) 
	{
		DetachLayer(l);
	}


	Layer* LayerStack::operator[](unsigned int index) 
	{
		return m_layers[index]; 
	}
}