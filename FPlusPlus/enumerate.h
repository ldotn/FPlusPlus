#pragma once
#include <tuple>
#include <algorithm>

namespace fpp
{
	namespace imp_
	{
		template<typename C>
		class enumerate
		{
		public:
			using value_type = std::tuple<size_t, typename C::value_type&&>;

			class InternalIterator
			{
			public:
				InternalIterator(typename C::iterator InIter) noexcept
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

				value_type operator*() { return value_type(Index, move(*Iter)); }
			private:
				size_t Index;
				typename C::iterator Iter;
			};

			using iterator = InternalIterator;

			enumerate(C& v)
				: Ref(v)
			{
				Count = std::size(v);
			}

			InternalIterator begin() noexcept { return InternalIterator(Ref.begin()); }
			InternalIterator end() noexcept { return InternalIterator(Ref.end()); }

			size_t size() const noexcept { return Count; }
		private:
			size_t Count;
			C& Ref;
		};
	}
	
	// Allows to iterate an std container and get the current index
	// Returns a tuple with the index and a reference to the value
	// For example
	//		a = { "a", "b", "c" }
	//		enumerate(a) 
	template<typename C>
	imp_::enumerate<C> enumerate(C& v) { return imp_::enumerate<C>(v); }
}