#pragma once
#include "AstralEngine/Math/AMath.h"
#include "ADynArr.h"
#include "ADelegate.h"

namespace AstralEngine
{
	class AABB
	{
	public:
		AABB(){ }

		AABB(const Vector2& min, const Vector2& max) : Min(min), Max(max) { }

		Vector2 Min;
		Vector2 Max;

		/// <summary>
		/// Checks if the provided AABB is completely contained within this AABB
		/// </summary>
		/// <param name="other">The other AABB to verify if it is inside this AABB</param>
		/// <returns>true if the provided AABB is entirely inside this AABB, false otherwise</returns>
		bool Contains(const AABB& other) const
		{
			return Min.x <= other.Min.x
				&& Min.y <= other.Min.y
				&& Max.x >= other.Max.x
				&& Max.y >= other.Max.y;
		}

		/// <summary>
		/// Checks if the provided AABB intersects or is contained within this AABB
		/// </summary>
		/// <param name="other">The AABB to verify if it intersects with this AABB</param>
		/// <returns>true if the provided AABB intersects or is entirely contained within this AABB</returns>
		bool Intersects(const AABB& other) const
		{
			return Min.x < other.Max.x 
				&& Max.x > other.Min.x 
				&& Min.y < other.Max.y 
				&& Max.y > other.Min.y;
		}
	};

	template<typename T>
	class QuadTree
	{
	public:
		QuadTree(const AABB& initialBounds, const ADelegate<AABB(const T&)>& getBounds) 
			: m_getBounds(getBounds), m_root(new Node())
		{
			m_root->bounds = initialBounds;
		}

		~QuadTree()
		{
			delete m_root;
		}

		void Add(const T& element)
		{
			AddInternal(m_root, element, m_getBounds(element));
		}

		void Remove(const T& element)
		{
			not done
		}

		ADynArr<T> Query(const AABB& bounds)
		{
			ADynArr<T> out;
			QueryNode(m_root, bounds, out);
			return out;
		}

	private:

		constexpr size_t s_maxNumDirectChildrenPerNode = 10;

		struct Node
		{
			T element;
			AABB bounds;
			ADynArr<Node*> children;
			// used when there are little to no children and for children on boundaries of nodes of our tree
			ADynArr<T> directChildren; 

			Node() : children(4) { }

			~Node()
			{
				for (Node* child in children)
				{
					delete child;
				}
			}
		};

		void QueryNode(Node* n, const AABB& queriedBounds, ADynArr<T>& out)
		{
			out->Reserve(n->directChildren.GetCount());

			for (const T& element : n->directChildren)
			{
				if (m_getBounds(element).Intersects(queriedBounds))
				{
					out.Add(element);
				}
			}

			for (Node* childNode : n)
			{
				if (childNode->bounds.Intersects(queriedBounds))
				{
					QueryNode(childNode, queriedBounds)
				}
			}
		}

		void AddInternal(Node* currNode, const T& element, const AABB& elementBounds)
		{
			AE_CORE_ASSERT(currNode->bounds.Contains(elementBounds), "");

			if (currNode->children.GetCount() == 0)
			{
				if (currNode->directChildren.GetCount() + 1 < s_maxNumDirectChildrenPerNode)
				{
					currNode->directChildren.Add(element);
				}
				else
				{
					DeepenNode(currNode, element, elementBounds);
				}
				return;
			}

			DistributeChildToChildNodes(n, element, n->directChildren);
		}

		void DeepenNode(Node* currNode, const T& element, const AABB& elementBounds)
		{
			AE_CORE_ASSERT(currNode->children.GetCount() == 0);
			
			// create child quadrants
			const AABB& parentBounds = currNode->bounds;

			float halfHeight = parentBounds.Min.y + 0.5f * (parentBounds.Max.y - parentBounds.Min.y);
			float halfWidth = parentBounds.Min.x + 0.5f * (parentBounds.Max.x - parentBounds.Min.x);

			Node* topLeft = new Node();
			topLeft->bounds = AABB(Vector2(parentBounds.Min.X, halfHeight), Vector2(halfWidth, parentBounds.Max.Y));

			Node* topRight = new Node();
			topRight->bounds = AABB(Vector2(halfWidth, halfHeight), parentBounds.Max);

			Node* bottomLeft = new Node();
			bottomLeft->bounds = AABB(parentBounds, Vector2(halfWidth, halfHeight));

			Node* bottomRight = new Node();
			bottomRight->bounds = AABB(Vector2(halfWidth, parentBounds.Min.y), Vector2(parentBounds.Max.x, halfHeight));

			currNode->children.Add(topLeft);
			currNode->children.Add(topRight);
			currNode->children.Add(bottomLeft);
			currNode->children.Add(bottomRight);


			// reassign direct children
			ADynArr<T> keptDirectChild;
			for (const T& directChild : currNode->directChildren)
			{
				DistributeChildToChildNodes(currNode, directChild, keptDirectChild);
			}

			// move new direct children into the parent node
			currNode->directChildren = std::move(keptDirectChild);
		}

		void DistributeChildToChildNodes(Node* n, const T& child, ADynArr<T>& keptDirectChild)
		{
			Node* nodeToContainChild = nullptr;
			AABB childBounds = m_getBounds(directChild);

			if (CheckBoundsInNode(n->children[0], directChild, childBounds, &nodeToContainChild, keptDirectChild))
			{
				return;
			}

			if (CheckBoundsInNode(n->children[1], directChild, childBounds, &nodeToContainChild, keptDirectChild))
			{
				return;
			}

			if (CheckBoundsInNode(n->children[2], directChild, childBounds, &nodeToContainChild, keptDirectChild))
			{
				return;
			}

			if (CheckBoundsInNode(n->children[3], directChild, childBounds, &nodeToContainChild, keptDirectChild))
			{
				return;
			}

			nodeToContainChild->directChildren.Add(directChild);
		}

		// returns true if can skip next checks
		bool CheckBoundsInNode(Node* n, const T& directChild, const AABB& childBounds, 
			Node** nodeToContainChild, ADynArr<T>& keptDirectChild)
		{
			if (n->bounds.Intersects(childBounds))
			{
				if (nodeToContainChild == nullptr)
				{
					*nodeToContainChild = topLeft;
				}
				else
				{
					keptDirectChild.Add(directChild);
					return true;
				}
			}
			return false;
		}

		Node* m_root;
		ADelegate<AABB(const T&)> m_getBounds;
	};
}