#pragma once
#include "ASinglyLinkedList.h"
#include <type_traits>
#include <tuple>
#include <functional>

namespace AstralEngine
{
	//used to wrap functions to pass as argument
	template<auto>
	struct FunctionWraper { };

	//used for readability when passing functions
	template<auto Function>
	const FunctionWraper<Function> function = {};

	template<typename...>
	class ADelegate;

	//specialization to force user to pass a function type
	template<typename Return, typename... Args>
	class ADelegate<Return(Args...)>
	{
		using Func = Return(Args...);
		using InternalFunc = Return(void*, Args...);

	public:
		ADelegate(std::nullptr_t) : m_func(nullptr), m_obj(nullptr) { }
		ADelegate() : m_func(nullptr), m_obj(nullptr) { }

		ADelegate(Func* function)
		{
			BindFunction(function);
		}

		ADelegate(InternalFunc* function, void* data = nullptr) 
			: m_func(function), m_obj(data) { }

		template<auto Function>
		ADelegate(FunctionWraper<Function>)
		{
			BindFunction<Function>();
		}

		ADelegate(const std::function<Return(Args...)>& func)
		{
			BindFunction(func);
		}


		template<auto Function, typename Type>
		ADelegate(FunctionWraper<Function>, Type& data)
		{
			BindFunction<Function>(&data);
		}

		template<auto Function, typename Type>
		ADelegate(FunctionWraper<Function>, Type* data)
		{
			BindFunction<Function>(data);
		}

		ADelegate(const ADelegate<Func>& other) 
			: m_func(other.m_func), m_obj(other.m_obj) { }

		ADelegate<Func>& BindFunction(Func* func)
		{
			
			m_obj = (void*)func;
			m_func = [](void* function, Args... args) -> Return
			{
				return ((Func*)function)(std::forward<Args>(args)...);
			};

			return *this;
		}

		ADelegate<Func>& BindFunction(InternalFunc* func, void* data = nullptr)
		{
			
			m_func = func;
			m_obj = data;

			return *this;
		}

		ADelegate<Func>& BindFunction(const std::function<Return(Args...)>& func)
		{
			
			/*extract function ptr from std::function and store it in m_obj to use in lambda
			*/
			m_obj = (void*)*func.target<Func*>();
			m_func = [](void* function, Args... args) -> Return
			{
				return ((Func*)function)(std::forward<Args>(args)...);
			};

			return *this;
		}

		template<auto Function>
		ADelegate<Func>& BindFunction()
		{
			
			AE_DATASTRUCT_ASSERT((std::is_invocable_r_v<Return, decltype(Function), Args...>),
				"Invalid function passed to delegate");
			m_obj = nullptr;
			m_func = [](void* obj, Args... args)
			{
				return Return(std::invoke(Function, std::forward<Args>(args)...));
			};

			return *this;
		}

		template<auto Function, typename Type>
		ADelegate<Func>& BindFunction(Type* obj)
		{
			
			AE_DATASTRUCT_ASSERT((std::is_invocable_r_v<Return, decltype(Function), Type&, Args...>),
				"Invalid Function type pair provided");
			m_obj = obj;
			m_func = [](void* o, Args... args) -> Return
			{
				Type* t = static_cast<Type*>(o);
				return Return(std::invoke(Function, *t, std::forward<Args>(args)...));
			};

			return *this;
		}

		void FreeFunction() 
		{
			m_func = nullptr;
			m_obj = nullptr;
		}

		InternalFunc* GetFunction() const { return m_func; }

		bool HasFunction() const { return m_func != nullptr; }

		Return operator()(Args... args) const
		{
			return m_func(m_obj, std::forward<Args>(args)...);
		}

		ADelegate<Return(Args...)>& operator=(const ADelegate<Return(Args...)>& other)
		{
			m_func = other.m_func;
			m_obj = other.m_obj;

			return *this;
		}

		ADelegate<Return(Args...)>& operator=(const std::function<Return(Args...)>& func)
		{
			BindFunction(func);
			return *this;
		}

		ADelegate<Return(Args...)>& operator=(Func* function)
		{
			BindFunction(function);
			return *this;
		}

		bool operator==(const ADelegate<Return(Args...)>& other) const
		{
			return m_func == other.m_func && m_obj == other.m_obj;
		}

		bool operator!=(const ADelegate<Return(Args...)>& other) const
		{
			return !(*this == other);
		}

		operator bool() const		
		{
			return HasFunction();
		}

		bool operator==(std::nullptr_t) const
		{
			return !HasFunction();
		}

		bool operator!=(std::nullptr_t) const
		{
			return !(*this == nullptr);
		}

	private:

		InternalFunc* m_func;
		void* m_obj;
	};

	//not defined so specialization below forces 
	//user to pass a function as template argument
	template<typename...>
	class SignalHandler;
	
	template<typename...>
	class Sink;

	template<typename Return, typename... Args>
	class SignalHandler<Return(Args...)>
	{
		friend class Sink<Return(Args...)>;

		using DelegateType = ADelegate<Return(Args...)>;
	public:

		void AddDelegate(const DelegateType& d)
		{
			m_delegates.Add(d);
		}

		void RemoveDelegate(const DelegateType& d)
		{
			m_delegates.Remove(d);
		}

		void CallDelagates(Args... args) const
		{
			for (DelegateType func : m_delegates)
			{
				func(std::forward<Args>(args)...);
			}
		}

		void Clear()
		{
			m_delegates.Clear();
		}

		size_t GetCount() const { return m_delegates.GetCount(); }

		bool IsEmpty() const { return m_delegates.IsEmpty(); }

		void operator()(Args... args) const
		{
			CallDelagates(std::forward<Args>(args)...);
		}

	private:
		ASinglyLinkedList<DelegateType> m_delegates;
	};


	/* wrapper object that contains a signal handler. Allows to pass the 
	   SignalHandler to a client class without allowing them to call the delegates 
	*/
	template<typename Return, typename... Args>
	class Sink<Return(Args...)>
	{
		using HandlerType = typename SignalHandler<Return(Args...)>;
		using DelegateType = typename HandlerType::DelegateType;

	public:

		Sink(HandlerType& handler) : m_handler(&handler) { }

		void AddDelegate(DelegateType d)
		{
			m_handler->AddDelegate(d);
		}

		void RemoveDelegate(DelegateType d)
		{
			m_handler->RemoveDelegate(d);
		}

		void Clear()
		{
			m_handler->Clear();
		}

		size_t GetCount() const { return m_handler->GetCount(); }

		bool IsEmpty() const { return m_handler->IsEmpty(); }

	private:
		HandlerType* m_handler;
	};

}