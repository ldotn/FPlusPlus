#pragma once
#include <tuple>
#include <algorithm>

namespace fpp
{
	namespace imp_
	{
		template<typename C0, typename C1>
		class zip
		{
		public:
			using value_type = typename std::tuple<typename C0::value_type&&, typename C1::value_type&&>;

			class InternalIterator
			{
			public:
				InternalIterator(typename  C0::iterator InIterA, typename  C1::iterator InIterB) noexcept
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

				value_type operator*() { return value_type(move(*IterA), move(*IterB)); }
			private:
				typename C0::iterator IterA;
				typename C1::iterator IterB;
			};

			using iterator = InternalIterator;

			zip(C0& a, C1& b)
				: RefA(a), RefB(b)
			{
				Count = std::min({ std::size(a),std::size(b) });
			}

			InternalIterator begin() noexcept { return InternalIterator(RefA.begin(), RefB.begin()); }
			InternalIterator end() noexcept { return InternalIterator(RefA.end(), RefB.end()); }

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
	imp_::zip<C0, C1> zip(C0& a, C1& b) { return imp_::zip<C0, C1>(a, b); }
}
