#pragma once
#include <tuple>
#include <algorithm>
#include <functional>

namespace fpp
{
	namespace imp_
	{
		template<typename C,typename CIter,typename CValue>
		class enumerate
		{
		public:
			using value_type = std::tuple<size_t, CValue&>;

			class InternalIterator
			{
			public:
				InternalIterator(CIter InIter) noexcept
					: Iter(InIter)
				{
					Index = 0;
				}

				InternalIterator& operator++()
				{
					Index++;
					Iter++;
					return *this;
				}
				InternalIterator operator++(int) // dummy parameter
				{
					Index++;
					Iter++;
					return *this;
				}

				inline bool operator!=(const InternalIterator& rhs) noexcept { return !(Iter == rhs.Iter); }

				value_type operator*() { return std::make_tuple(Index, std::ref(*Iter)); }
			private:
				size_t Index;
				CIter Iter;
			};

			using iterator = InternalIterator;

			enumerate(C& v)
				: Ref(v)
			{
				Count = std::size(v);
			}

			InternalIterator begin() noexcept { return InternalIterator(std::begin(Ref)); }
			InternalIterator end() noexcept { return InternalIterator(std::end(Ref)); }

			size_t size() const noexcept { return Count; }
		private:
			size_t Count;
			C& Ref;
		};
	}
	
	// Allows to iterate an std container or C array and get the current index
	// Returns a tuple with the index and a reference to the value
	// For example
	//		a = { "a", "b", "c" }
	//		enumerate(a) 
	template<typename C>
	auto enumerate(C& v) { return imp_::enumerate<C,decltype(std::begin(v)),decltype(*std::begin(v))>(v); }
}