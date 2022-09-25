#pragma once

/*---------------------------------------------------------------------------------------------
�̸�     : MemoryPoolSTL
�뵵     : GMemoryManger�� �̿��Ͽ� STL�� �޸𸮵� ���� �ϴ� ��ü
������   : �̹α�
������¥ : 2022.08.12
----------------------------------------------------------------------------------------------*/

#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <array>

/*---------------------------------------------------------------------------------------------
�̸�     : STLAllocate
�뵵     : STL�� Allocate�� �����ϱ� ���� ��ü
������   : �̹α�
������¥ : 2022.08.12
----------------------------------------------------------------------------------------------*/
template<typename T>
class STLAllocate
{
public:
	using value_type = T;

	STLAllocate() { }

	template<typename Other>
	STLAllocate(const STLAllocate<Other>&) { }

	T* allocate(size_t count)
	{
		const int32 size = static_cast<int32>(count * sizeof(T));
		return static_cast<T*>(GMemoryManager->Pop(size));
	}

	void deallocate(T * ptr, size_t count)
	{
		using DataType = std::remove_reference_t<T>;
		GMemoryManager->push(ptr , count * sizeof(DataType));
	}
};

template<typename Type>
using Gvector = vector<Type, STLAllocate<Type>>;

template<typename Type>
using Glist = list<Type, STLAllocate<Type>>;

template<typename Key, typename Type, typename Pred = less<Key>>
using Gmap = map<Key, Type, Pred, STLAllocate<pair<const Key, Type>>>;

template<typename Key, typename Pred = less<Key>>
using Gset = set<Key, Pred, STLAllocate<Key>>;

template<typename Type>
using Gdeque = deque<Type, STLAllocate<Type>>;

template<typename Type, typename Container = Gdeque<Type>>
using Gqueue = queue<Type, Container>;

template<typename Type, typename Container = Gdeque<Type>>
using Gstack = stack<Type, Container>;

template<typename Type, typename Container = Gvector<Type>, typename Pred = greater<typename Container::value_type>>
using GpriorityQueue = priority_queue<Type, Container, Pred>;

using Gstring = basic_string<char, char_traits<char>, STLAllocate<char>>;

using GWString = basic_string<wchar_t, char_traits<wchar_t>, STLAllocate<wchar_t>>;

template<typename Key, typename Type, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using GhashMap = unordered_map<Key, Type, Hasher, KeyEq, STLAllocate<pair<const Key, Type>>>;

template<typename Key, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using GhashSet = unordered_set<Key, Hasher, KeyEq, STLAllocate<Key>>;