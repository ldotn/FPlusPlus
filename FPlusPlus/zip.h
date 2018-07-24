#pragma once
#include <tuple>
#include <algorithm>

namespace fpp
{
	namespace imp_
	{
		template<typename C0, typename C1, typename C0Iter, typename C0Value, typename C1Iter, typename C1Value>
		class zip
		{
		public:
			using value_type = typename std::tuple<C0Value&,C1Value&>;

			class InternalIterator
			{
			public:
				InternalIterator(C0Iter InIterA, C1Iter InIterB) noexcept
					: IterA(InIterA), IterB(InIterB)
				{
				}

				InternalIterator& operator++()
				{
					IterA++;
					IterB++;
					return *this;
				}
				InternalIterator operator++(int)
				{
					IterA++;
					IterB++;
					return *this;
				}

				// Abusing the implementation for performance here
				// The != operator is used to compare with end. 
				// Because we are zipping things of potentially different sizes, we need to break if any of them is equal to the end
				// But that's not the behavior you would expect from !=
				inline bool operator!=(const InternalIterator& rhs) noexcept { return !(IterA == rhs.IterA || IterB == rhs.IterB); }

				value_type operator*() { return std::make_tuple(std::ref(*IterA), std::ref(*IterB)); }
			private:
				C0Iter IterA;
				C1Iter IterB;
			};

			using iterator = InternalIterator;

			zip(C0& a, C1& b)
				: RefA(a), RefB(b)
			{
				Count = std::min({ std::size(a),std::size(b) });
			}

			InternalIterator begin() noexcept { return InternalIterator(std::begin(RefA), std::begin(RefB)); }
			InternalIterator end() noexcept { return InternalIterator(std::end(RefA), std::end(RefB)); }

			size_t size() const noexcept { return Count; }
		private:
			size_t Count;
			C0& RefA;
			C1& RefB;
		};
	}

	// Allows iterating two std containers at the same time
	// Returns a tuple with references
	// Stops when either one reaches the end
	// For example
	//		a = { 1, 2, 3 }
	//		b = { 4, 5, 6, 7 }
	//	zip(a,b) -> (1,4) , (2,5) , (3,6)
	template<typename C0, typename C1>
	auto zip(C0& a, C1& b) 
	{ 
		return imp_::zip
			<
				C0, C1, 
				decltype(std::begin(a)), decltype(*std::begin(a)), 
				decltype(std::begin(b)), decltype(*std::begin(b))
			> (a, b); 
	}
}
