#pragma once
#include "qspi_flash_driver.hh"

// FlashBlock<qspi.get_32kblock_addr(3), MyDataStruct, QSPI_32KBLOCK_SIZE, QSPI_PAGE_SIZE> my_block;

// Wrapper reader/writer for a QSPI Flash block of size BlockSize. The block is split into
// [cell_nr_] cells of equal size, each potentially containing an object of
// Data, and aligned to page boundaries
template<int BlockAddr, class DataT, int BlockSize, int PageSize>
struct FlashCellBlock {
	FlashCellBlock(QSpiFlash &qspi)
		: qspi_(qspi)
	{}
	QSpiFlash &qspi_;
	static constexpr int data_size_ = sizeof(DataT);

	// data size aligned to the next page boundary
	static_assert(MathTools::is_power_of_2(PageSize));
	static constexpr int page_size_bits_ = MathTools::Log2<PageSize>::val;
	static constexpr int aligned_data_size_ = ((data_size_ >> page_size_bits_) + 1)
											  << page_size_bits_;
	static_assert(aligned_data_size_ < BlockSize);
	static constexpr int cell_nr_ = BlockSize / aligned_data_size_;

	bool Read(DataT *data, int cell = 0)
	{
		if (cell >= cell_nr_)
			return false;
		uint32_t addr = BlockAddr + cell * aligned_data_size_;
		while (!qspi_.is_ready())
			;
		bool read_ok = qspi_.Read_Background(reinterpret_cast<uint8_t *>(data), addr, data_size_);
		while (!qspi_.is_ready())
			;
		return read_ok;
	}

	bool Write(DataT *data, int cell = 0)
	{
		if (cell >= cell_nr_)
			return false;
		uint32_t addr = BlockAddr + cell * aligned_data_size_;
		while (!qspi_.is_ready())
			;
		return qspi_.Write(reinterpret_cast<uint8_t *>(data), addr, data_size_);
	}

	bool Erase()
	{
		while (!qspi_.is_ready())
			;
		return qspi_.Erase_Background(BlockSize, BlockAddr);
	}
	
	// Verify all bits are 1's
	bool IsWriteable(int cell)
	{
		if (cell >= cell_nr_)
			return false;
		uint8_t check[data_size_];
		if (Read(reinterpret_cast<DataT *>(check), cell)) {
			for (int i = 0; i < data_size_; i++) {
				if (check[i] != 0xFF)
					return false;
			}
			return true;
		}
		return false;
	}
};

