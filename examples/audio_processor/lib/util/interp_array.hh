#pragma once

template<typename T, unsigned int Size, typename PhaseT = float>
struct InterpArray {
	T data[Size];

	T &operator[](const unsigned int index)
	{
		return data[index];
	}
	T &set(const unsigned int index)
	{
		return data[index];
	}
	void set(const unsigned int index, T val)
	{
		data[index] = val;
	}
	const T operator[](const unsigned int index) const
	{
		return data[index];
	}
	const T get(const unsigned int index) const
	{
		return data[index];
	}

	// Return an interpolated value where index = 0 and index = Size are the first element
	constexpr T interp_by_index_wrap(const PhaseT index) const
	{
		unsigned int idx0 = static_cast<unsigned int>(index);
		const auto phase = index - static_cast<PhaseT>(idx0);
		while (idx0 >= Size)
			idx0 -= Size;
		const unsigned int idx1 = (idx0 == (Size - 1)) ? 0 : idx0 + 1;

		return (data[idx1] * phase) + (data[idx0] * ((PhaseT)(1.) - phase));
		// return data[idx0] + (data[idx1] - data[idx0]) * phase;
		// return data[idx1] * index - data[idx1] * idx0 + data[idx0] - data[idx0] * index + data[idx0]*idx0;
	}

	// Return an interpolated value. index = 0 returns the first element and index = Size-1 returns the last element
	// Boundary checking is not done
	// Todo: check if the unsigned int cast uses the native ARM float32->int32 conversion. If not, figure out how to do
	// that while keeping portability
	constexpr T interp_by_index(const PhaseT index) const
	{
		auto idx0 = static_cast<unsigned int>(index);
		const auto phase = index - static_cast<PhaseT>(idx0);
		const unsigned int idx1 = (idx0 == (Size - 1)) ? 0 : idx0 + 1;

		// return data[idx0] + (data[idx1] - data[idx0]) * phase;
		return (data[idx1] * phase) + (data[idx0] * ((PhaseT)(1.) - phase));
	}

	// Return an interpolated value where phase = 0 and phase = N (any integer) return the first element
	constexpr T interp_wrap(const PhaseT phase) const
	{
		const auto index = phase * Size;
		return interp_by_index_wrap(index);
	}

	// Return an interpolated value where phase = 0 returns the first element, and phase = 1 returns the last element
	// phase > 1 or phase < 0 will cause undefined behavior! Caller must ensure 0 <= phase =< 1.
	// Use interp_wrap() if you can't ensure this.
	constexpr T interp(const PhaseT phase) const
	{
		const PhaseT index = phase * (Size - 1);
		return interp_by_index(index);
	}

	// Todo: this is really "floor" not "closest" (it doesn't round)
	// phase = 0 and phase = 1, 2, 3.. return first element
	constexpr T closest_wrap(const PhaseT phase) const
	{
		auto idx = static_cast<unsigned int>(phase * Size);
		while (idx >= Size)
			idx -= Size;
		return data[idx];
	}

	// Todo: Rename this, as it's really "floor" not "closest" (it doesn't round)
	// phase = 0 returns first element, phase = 1 returns last element
	// No boundary checking is done
	constexpr T closest(const PhaseT phase) const
	{
		const auto idx = static_cast<unsigned int>(phase * (Size - 1));
		return data[idx];
	}
};
