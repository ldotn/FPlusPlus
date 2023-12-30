#pragma once
#include <algorithm>
#include <functional>
#include <int2.h>

// Row major container for dynamic 2D arrays
// It has unique_ptr like semantics, in that assignment invalidates the object on the right (technically it just swaps them)
template<typename T>
struct Buffer2D
{
	int2 offset;
	int2 res;
	T* data;

	Buffer2D() { data = nullptr; }
	Buffer2D(int2 _res)
	{
		data = nullptr;
		resize(_res);
		offset.x = 0;
		offset.y = 0;
	}

	~Buffer2D()
	{
		if (data) delete[] data;
	}

	Buffer2D(const Buffer2D& rhs)
	{
		data = nullptr;
		resize(rhs.res);
		for (int i = 0; i < res.x*res.y; i++)
			data[i] = rhs.data[i];
		offset = rhs.offset;
	}

	Buffer2D(int2 _res, T val, int) // dummy parameter at the end to resolve ambiguities
	{
		data = nullptr;
		resize(_res);

		offset.x = 0;
		offset.y = 0;

		foreach([&val](int, int2, T& v) { v = val; });
	}

	Buffer2D(Buffer2D&& rhs) : Buffer2D() // initialize via default constructor, C++11 only
	{
		swap(*this, rhs);
	}

	Buffer2D& operator=(Buffer2D rhs)
	{
		swap(*this, rhs);
		return *this;
	}

	friend void swap(Buffer2D& lhs, Buffer2D& rhs)
	{
		using std::swap; // allow use of std::swap...
		swap(lhs.res, rhs.res);
		swap(lhs.data, rhs.data);
		swap(lhs.offset, rhs.offset);
	}

	Buffer2D<T> duplicate() const
	{
		Buffer2D<T> out(res);
		memcpy(out.data, data, sizeof(T) * res.x * res.y);

		return out;
	}

	void resize(int2 newRes)
	{
		if (data) delete[] data;

		res = newRes;
		data = new T[res.x*res.y];
	}

	T& operator[](int2 p)
	{
		p.x = std::clamp<int>(p.x - offset.x, 0, res.x - 1);
		p.y = std::clamp<int>(p.y - offset.y, 0, res.y - 1);

		return data[p.x + res.x*p.y];
	}
	const T& operator[](int2 p) const
	{
		p.x = std::clamp<int>(p.x - offset.x, 0, res.x - 1);
		p.y = std::clamp<int>(p.y - offset.y, 0, res.y - 1);

		return data[p.x + res.x*p.y];
	}

	void foreach(std::function<void(int, int2, T&)> f)
	{
		int idx = 0;

		for (int y = 0; y < res.y; y++)
		{
			for (int x = 0; x < res.x; x++)
			{
				f(idx, { x,y }, data[idx]);
				idx++;
			}
		}
	}
	
	bool contains(int2 px) const
	{
		px -= offset;
		return (px.x >= 0 && px.x < res.x) && (px.y >= 0 && px.y < res.y);
	}

	void foreach(std::function<void(int, int2, T)> f) const
	{
		int idx = 0;

		for (int y = 0; y < res.y; y++)
		{
			for (int x = 0; x < res.x; x++)
			{
				f(idx, { x,y }, data[idx]);
				idx++;
			}
		}
	}

	// like foreach but you can early exit
	void iterate(std::function<bool(int, int2, T)> f) const
	{
		int idx = 0;

		bool exit_loop = false;
		for (int y = 0; y < res.y && !exit_loop; y++)
		{
			for (int x = 0; x < res.x; x++)
			{
				if (f(idx, { x,y }, data[idx]))
				{
					exit_loop = true;
					break;
				}
				idx++;
			}
		}
	}

	void window_map(int radius, std::function<bool(int2, int2, T)> pix_f, std::function<bool(int2)> window_pre_f, std::function<bool(int2)> window_post_f = std::function<bool(int2)>() )
	{
		bool exit_loop = false;
		for (int y = 0; y < res.y && !exit_loop; y++)
		{
			for (int x = 0; x < res.x; x++)
			{
				if (window_pre_f({ x,y }))
				{
					exit_loop = true;
					break;
				}

				bool exit_sample = false;
				for (int rx = -radius; rx <= radius && !exit_sample; rx++)
				{
					for (int ry = -radius; ry <= radius; ry++)
					{
						// TODO : Move clamp to function
						int sx = std::clamp<int>((x + rx) - offset.x, 0, res.x - 1);
						int sy = std::clamp<int>((y + ry) - offset.y, 0, res.y - 1);

						if (pix_f({ sx, sy }, { rx, ry }, data[sx + res.x * sy]))
						{
							exit_sample = true;
							break;
						}
					}
				}

				if (window_post_f({ x,y }))
				{
					exit_loop = true;
					break;
				}
			}
		}
	}
};