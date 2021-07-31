#pragma once
#include "arch.hh"
#include "rcc.hh"
#include "spi_config_struct.hh"
#include "spi_registers.hh"
#include "util/math.hh"

// Todo: move to target-specific dir (is shared between H7 and MP1, so one can #include the other)
namespace mdrivlib
{

template<typename ConfT>
struct SpiPeriph {
public:
	static inline const unsigned N = ConfT::PeriphNum;

	template<unsigned M>
	using IER = typename SPI_<N>::template IER<M>;
	template<unsigned M>
	using CR1 = typename SPI_<N>::template CR1<M>;
	template<unsigned M>
	using CR2 = typename SPI_<N>::template CR2<M>;
	template<unsigned M>
	using SR = typename SPI_<N>::template SR<M>;
	template<unsigned M>
	using IFCR = typename SPI_<N>::template IFCR<M>;
	template<unsigned M>
	using CFG1 = typename SPI_<N>::template CFG1<M>;
	template<unsigned M>
	using CFG2 = typename SPI_<N>::template CFG2<M>;
	template<unsigned M>
	using CRCPOLY = typename SPI_<N>::template CRCPOLY<M>;

	SpiPeriph() = default;

	void configure() {
		disable();

		Pin init_sclk{ConfT::SCLK, PinMode::Alt};
		Pin init_copi{ConfT::COPI, PinMode::Alt};
		Pin init_cipo{ConfT::CIPO, PinMode::Alt};

		static_assert(!ConfT::use_hardware_ss ||
						  (ConfT::use_hardware_ss && (ConfT::NumChips == 1) && (ConfT::CS0.af > 0)),
					  "Hardware SS only supported when NumChips = 1, and pin AF > 0");

		if (ConfT::use_hardware_ss) {
			Pin init_nss{ConfT::CS0, PinMode::Alt};
		} else {
			Pin init_cs0{ConfT::CS0, PinMode::Output};
			unselect<0>();
			if constexpr (ConfT::NumChips > 1) {
				Pin init_cs1{ConfT::CS1, PinMode::Output};
				unselect<1>();
			}
			if constexpr (ConfT::NumChips > 2) {
				Pin init_cs2{ConfT::CS2, PinMode::Output};
				unselect<2>();
			}
			if constexpr (ConfT::NumChips > 3) {
				Pin init_cs3{ConfT::CS3, PinMode::Output};
				unselect<3>();
			}
			static_assert(ConfT::NumChips <= 4, "mdrivlib::SpiPeriph only supports selecting 1-4 chips");
		}

		RCC_Enable::SPI<N>::set();

		// Todo: make configurable
		CR1<SPI_CR1_IOLOCK>::clear();

		if constexpr (ConfT::data_dir == SpiDataDir::Duplex)
			CFG2<SPI_CFG2_COMM>::write(0);
		if constexpr (ConfT::data_dir == SpiDataDir::TXOnly)
			CFG2<SPI_CFG2_COMM>::write(0b01 << SPI_CFG2_COMM_Pos);
		if constexpr (ConfT::data_dir == SpiDataDir::RXOnly)
			CFG2<SPI_CFG2_COMM>::write(0b10 << SPI_CFG2_COMM_Pos);
		if constexpr (ConfT::data_dir == SpiDataDir::HalfDuplex)
			CFG2<SPI_CFG2_COMM>::write(0b11 << SPI_CFG2_COMM_Pos);

		set_data_size(ConfT::data_size);
		// or: CFG1<SPI_CFG1_DSIZE>::write((ConfT::data_size - 1) << SPI_CFG1_DSIZE_Pos);

		if constexpr (ConfT::clock_high_when_idle)
			CFG2<SPI_CFG2_CPOL>::set();
		else
			CFG2<SPI_CFG2_CPOL>::clear();

		if constexpr (ConfT::second_clk_transition_captures_data)
			CFG2<SPI_CFG2_CPHA>::set();
		else
			CFG2<SPI_CFG2_CPHA>::clear();

		if constexpr (ConfT::use_hardware_ss) {
			CFG2<SPI_CFG2_SSOE>::set();
			CFG2<SPI_CFG2_SSM>::clear();
			// CFG2<SPI_CFG2_SSOM>::write(ConfT::pulse_hardware_ss ? SPI_CFG2_SSOM : 0);
			if constexpr (ConfT::pulse_hardware_ss)
				CFG2<SPI_CFG2_SSOM>::set();
			else
				CFG2<SPI_CFG2_SSOM>::clear();
		} else {
			CFG2<SPI_CFG2_SSOE>::clear();
			CFG2<SPI_CFG2_SSM>::set();
		}

		if constexpr (!ConfT::use_hardware_ss && !ConfT::SS_active_high && ConfT::is_controller)
			CR1<SPI_CR1_SSI>::set();
		else
			CR1<SPI_CR1_SSI>::clear();

		if constexpr (ConfT::SS_active_high)
			CFG2<SPI_CFG2_SSIOP>::set();
		else
			CFG2<SPI_CFG2_SSIOP>::clear();

		// FixMe: This must be set/cleared AFTER CR1_SSI is set/cleared. Why?
		if constexpr (ConfT::is_controller)
			CFG2<SPI_CFG2_MASTER>::set();
		else
			CFG2<SPI_CFG2_MASTER>::clear();

		if constexpr (ConfT::clock_division < 2)
			CFG1<SPI_CFG1_MBR>::write(0b000 << SPI_CFG1_MBR_Pos);
		else if constexpr (ConfT::clock_division > 256)
			CFG1<SPI_CFG1_MBR>::write(0b111 << SPI_CFG1_MBR_Pos);
		else
			CFG1<SPI_CFG1_MBR>::write((MathTools::log2_floor(ConfT::clock_division) - 1) << SPI_CFG1_MBR_Pos);

		if constexpr (ConfT::LSBfirst)
			CFG2<SPI_CFG2_LSBFRST>::set();
		else
			CFG2<SPI_CFG2_LSBFRST>::clear();

		// Todo: make configurable
		// TI Mode
		CFG2<SPI_CFG2_SP>::write(0);

		if constexpr (ConfT::EnableCRC) {
			CFG1<SPI_CFG1_CRCEN>::set();
			CFG1<SPI_CFG1_CRCSIZE>::write(ConfT::CRCSize);
			CRCPOLY<SPI_CRCPOLY_CRCPOLY>::write(ConfT::CRCPoly);
			// Todo: make configurable
			CR1<SPI_CR1_TCRCINI>::clear();
			// Todo: make configurable
			CR1<SPI_CR1_RCRCINI>::clear();
		} else
			CFG1<SPI_CFG1_CRCEN>::clear();

		CFG1<SPI_CFG1_FTHLV>::write((ConfT::FifoThreshold - 1) << SPI_CFG1_FTHLV_Pos);

		CFG2<SPI_CFG2_MSSI>::write((ConfT::NumClocksDelayBeforeData & SPI_CFG2_MSSI_Msk) << SPI_CFG2_MSSI_Pos);

		CFG2<SPI_CFG2_MIDI>::write((ConfT::NumClocksToggleSSInterData & 0b1111) << SPI_CFG2_MIDI_Pos);

		// Todo: make configurable
		CR1<SPI_CR1_MASRX>::clear();

		// Todo: make configurable
		CFG2<SPI_CFG2_AFCNTR>::set();

		// Todo: make configurable
		CFG2<SPI_CFG2_IOSWP>::clear();
	}

	void enable() {
		CR1<SPI_CR1_SPE>::set();
	}
	void disable() {
		CR1<SPI_CR1_SPE>::clear();
	}
	// Interrupts
	void enable_RX_interrupt() {
		IER<SPI_IER_RXPIE>::set();
	}
	void disable_RX_interrupt() {
		IER<SPI_IER_RXPIE>::clear();
	}
	void enable_TX_interrupt() {
		IER<SPI_IER_TXPIE>::set();
	}
	void disable_TX_interrupt() {
		IER<SPI_IER_TXPIE>::clear();
	}
	void enable_duplex_interrupt() {
		IER<SPI_IER_DXPIE>::set();
	}
	void disable_duplex_interrupt() {
		IER<SPI_IER_DXPIE>::clear();
	}
	void enable_end_of_xfer_interrupt() {
		IER<SPI_IER_EOTIE>::set();
	}
	void disable_end_of_xfer_interrupt() {
		IER<SPI_IER_EOTIE>::clear();
	}
	void clear_EOT_flag() {
		IFCR<SPI_IFCR_EOTC>::set();
	}
	void clear_TXTF_flag() { // transmission transfer filled
		IFCR<SPI_IFCR_TXTFC>::set();
	}

	void enable_tx_dma() {
		CFG1<SPI_CFG1_TXDMAEN>::set();
	}
	uint32_t get_tx_datareg_addr() {
		return reinterpret_cast<uint32_t>(SPI_<N>::TXDR::BaseAddress);
	}

	// Status flags
	bool rx_packet_available() {
		return SR<SPI_SR_RXP>::read() ? true : false;
	}
	bool tx_space_available() {
		return SR<SPI_SR_TXP>::read() ? true : false;
	}
	bool duplex_space_available() {
		return SR<SPI_SR_DXP>::read() ? true : false;
	}
	bool is_end_of_transfer() {
		return SR<SPI_SR_EOT>::read() ? true : false;
	}
	bool is_tx_complete() {
		return SR<SPI_SR_TXC>::read() ? true : false;
	}
	bool rx_fifo_not_empty() {
		return SR<SPI_SR_RXWNE>::read() ? true : false;
	}
	int rx_fifo_frames_left() {
		return SR<SPI_SR_RXPLVL>::read() >> SPI_SR_RXPLVL_Pos;
	}

	// TX conditions
	void set_tx_message_size(uint16_t num_packets) {
		CR2<SPI_CR2_TSIZE>::write(num_packets);
	}
	void set_tx_message_reload_size(uint16_t num_packets) {
		CR2<SPI_CR2_TSER>::write(num_packets);
	}
	void set_data_size(uint8_t data_size) {
		CFG1<SPI_CFG1_DSIZE>::write((data_size - 1) << SPI_CFG1_DSIZE_Pos);
	}
	void set_fifo_threshold(uint8_t num_bytes) {
		CFG1<SPI_CFG1_FTHLV>::write((num_bytes - 1) << SPI_CFG1_FTHLV_Pos);
	}
	void load_tx_data(uint16_t data0, uint16_t data1) {
		SPI_<N>::TXDR::write(data0 << 16 | data1);
	}
	void load_tx_data(uint32_t data) {
		// Todo: Do we need to have variable-sized writes?
		// if constexpr (ConfT::data_size == 16)
		// 	SPI_<N>::TXDR_16::write((uint16_t)data);
		// if constexpr (ConfT::data_size == 8)
		// 	SPI_<N>::TXDR_8::write((uint8_t)data);
		// if constexpr (ConfT::data_size > 16)
		SPI_<N>::TXDR::write(data);
	}
	void start_transfer() {
		CR1<SPI_CR1_CSTART>::set();
	}
	uint16_t received_data() {
		return SPI_<N>::RXDR::read();
	}

	// Half-duplex mode
	void start_half_duplex_tx() {
		if constexpr (ConfT::data_dir == SpiDataDir::HalfDuplex) {
			CR1<SPI_CR1_HDDIR>::set();
		}
	}
	void start_half_duplex_rx() {
		if constexpr (ConfT::data_dir == SpiDataDir::HalfDuplex) {
			CR1<SPI_CR1_HDDIR>::clear();
		}
	}

	void enable_auto_suspend() {
		if constexpr (ConfT::data_dir != SpiDataDir::TXOnly && ConfT::is_controller) {
			CR1<SPI_CR1_MASRX>::set();
		}
	}

	template<int chip_num>
	void select() {
		if constexpr (chip_num == 0)
			FPin<ConfT::CS0.gpio, ConfT::CS0.pin, PinMode::Output>::low();
		if constexpr (chip_num == 1)
			FPin<ConfT::CS1.gpio, ConfT::CS1.pin, PinMode::Output>::low();
		if constexpr (chip_num == 2)
			FPin<ConfT::CS2.gpio, ConfT::CS2.pin, PinMode::Output>::low();
		if constexpr (chip_num == 3)
			FPin<ConfT::CS3.gpio, ConfT::CS3.pin, PinMode::Output>::low();
	}
	template<int chip_num>
	void unselect() {
		if constexpr (chip_num == 0)
			FPin<ConfT::CS0.gpio, ConfT::CS0.pin, PinMode::Output>::high();
		if constexpr (chip_num == 1)
			FPin<ConfT::CS1.gpio, ConfT::CS1.pin, PinMode::Output>::high();
		if constexpr (chip_num == 2)
			FPin<ConfT::CS2.gpio, ConfT::CS2.pin, PinMode::Output>::high();
		if constexpr (chip_num == 3)
			FPin<ConfT::CS3.gpio, ConfT::CS3.pin, PinMode::Output>::high();
	}
	void select(int chip_num) {
		if (chip_num == 0)
			select<0>();
		if (chip_num == 1)
			select<1>();
		if (chip_num == 2)
			select<2>();
		if (chip_num == 3)
			select<3>();
	}
	void unselect(int chip_num) {
		if (chip_num == 0)
			unselect<0>();
		if (chip_num == 1)
			unselect<1>();
		if (chip_num == 2)
			unselect<2>();
		if (chip_num == 3)
			unselect<3>();
	}

private:
	FPin<ConfT::CS0.gpio, ConfT::CS0.pin> CS0;
	FPin<ConfT::CS1.gpio, ConfT::CS1.pin> CS1;
	FPin<ConfT::CS2.gpio, ConfT::CS2.pin> CS2;
	FPin<ConfT::CS3.gpio, ConfT::CS3.pin> CS3;
};
} // namespace mdrivlib
