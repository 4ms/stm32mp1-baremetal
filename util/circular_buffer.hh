#pragma once
#include <array>
#include <cstdio>
#include <memory>

template<class T, size_t max_size_>
class CircularBuffer {
public:
	CircularBuffer(){};

	void put(T item)
	{
		buf_[head_] = item;

		if (full_) {
			tail_ = (tail_ + 1) % max_size_;
		}

		head_ = (head_ + 1) % max_size_;
		full_ = head_ == tail_;
	}

	T get()
	{
		if (empty()) {
			return T();
		}

		// Read data and advance the tail (we now have a free space)
		auto val = buf_[tail_];
		full_ = false;
		tail_ = (tail_ + 1) % max_size_;

		return val;
	}

	void set_head(size_t head)
	{
		head_ = head;
	}

	void remove_first()
	{
		if (!empty()) {
			full_ = false;
			tail_ = (tail_ + 1) % max_size_;
		}
	}

	// Return a reference to the first element
	T &first()
	{
		return buf_[tail_];
	}

	void reset()
	{
		head_ = tail_;
		full_ = false;
	}

	bool empty() const
	{
		// if head and tail are equal, we are empty
		return (!full_ && (head_ == tail_));
	}

	bool full() const
	{
		return full_;
	}

	size_t capacity() const
	{
		return max_size_;
	}

	size_t size() const
	{
		size_t size = max_size_;

		if (!full_) {
			if (head_ >= tail_)
				size = head_ - tail_;
			else
				size = max_size_ + head_ - tail_;
		}

		return size;
	}

private:
	std::array<T, max_size_> buf_{};
	size_t head_ = 0;
	size_t tail_ = 0;
	bool full_ = 0;
};

