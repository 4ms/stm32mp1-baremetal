#pragma once
#include "register_access.hh"
#include "stm32xx.h"

namespace mdrivlib
{
template<unsigned ChannelNum>
struct MDMA_ {
	static constexpr uint32_t BASE = ChannelNum == 0  ? MDMA_Channel0_BASE
								   : ChannelNum == 1  ? MDMA_Channel1_BASE
								   : ChannelNum == 2  ? MDMA_Channel2_BASE
								   : ChannelNum == 3  ? MDMA_Channel3_BASE
								   : ChannelNum == 4  ? MDMA_Channel4_BASE
								   : ChannelNum == 5  ? MDMA_Channel5_BASE
								   : ChannelNum == 6  ? MDMA_Channel6_BASE
								   : ChannelNum == 7  ? MDMA_Channel7_BASE
								   : ChannelNum == 8  ? MDMA_Channel8_BASE
								   : ChannelNum == 9  ? MDMA_Channel9_BASE
								   : ChannelNum == 10 ? MDMA_Channel10_BASE
								   : ChannelNum == 11 ? MDMA_Channel11_BASE
								   : ChannelNum == 12 ? MDMA_Channel12_BASE
								   : ChannelNum == 13 ? MDMA_Channel13_BASE
								   : ChannelNum == 14 ? MDMA_Channel14_BASE
													  : MDMA_Channel15_BASE;

	static constexpr uint32_t CISR_Base = BASE + offsetof(MDMA_Channel_TypeDef, CISR);
	using RequestIsActive = RegisterSection<ReadOnly, CISR_Base, MDMA_CISR_CRQA_Pos, 1>;
	using BufferTransferComplISRFlag = RegisterSection<ReadWrite, CISR_Base, MDMA_CISR_TCIF_Pos, 1>;
	using BlockTransferComplISRFlag = RegisterSection<ReadWrite, CISR_Base, MDMA_CISR_BTIF_Pos, 1>;
	using BlockRepeatTransferComplISRFlag = RegisterSection<ReadWrite, CISR_Base, MDMA_CISR_BRTIF_Pos, 1>;
	using ChannelTransferComplISRFlag = RegisterSection<ReadWrite, CISR_Base, MDMA_CISR_CTCIF_Pos, 1>;
	using TransferErrISRFlag = RegisterSection<ReadWrite, CISR_Base, MDMA_CISR_TEIF_Pos, 1>;

	static constexpr uint32_t CIFCR_Base = BASE + offsetof(MDMA_Channel_TypeDef, CIFCR);
	using BufferTransferComplISRClear = RegisterSection<ReadWrite, CIFCR_Base, MDMA_CIFCR_CLTCIF_Pos, 1>;
	using BlockTransferComplISRClear = RegisterSection<ReadWrite, CIFCR_Base, MDMA_CIFCR_CBTIF_Pos, 1>;
	using BlockRepeatTransferComplISRClear = RegisterSection<ReadWrite, CIFCR_Base, MDMA_CIFCR_CBRTIF_Pos, 1>;
	using ChannelTransferComplISRClear = RegisterSection<ReadWrite, CIFCR_Base, MDMA_CIFCR_CCTCIF_Pos, 1>;
	using TransferErrISRClear = RegisterSection<ReadWrite, CIFCR_Base, MDMA_CIFCR_CTEIF_Pos, 1>;

	static constexpr uint32_t CESR_Base = BASE + offsetof(MDMA_Channel_TypeDef, CESR);
	// Todo

	static constexpr uint32_t CCR_Base = BASE + offsetof(MDMA_Channel_TypeDef, CCR);
	using SWRequest = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_SWRQ_Pos, 1>;
	using WordEndianessExchange = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_WEX_Pos, 1>;
	using HalfwordEndianessExchange = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_HEX_Pos, 1>;
	using ByteEndianessExchange = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_BEX_Pos, 1>;
	using PriorityLevel = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_PL_Pos, 1>;
	using BufferTransferComplISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_TCIE_Pos, 1>;
	using BlockTransferComplISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_BTIE_Pos, 1>;
	using BlockRepeatTransferComplISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_BRTIE_Pos, 1>;
	using ChannelTransferComplISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_CTCIE_Pos, 1>;
	using TransferErrISREnable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_TEIE_Pos, 1>;
	using Enable = RegisterSection<ReadWrite, CCR_Base, MDMA_CCR_EN_Pos, 1>;

	static constexpr uint32_t CTCR_Base = BASE + offsetof(MDMA_Channel_TypeDef, CTCR);
	using BufferableWriteMode = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_BWM_Pos, 1>;
	using SWRequestMode = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_SWRM_Pos, 1>;
	using TriggerMode = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_TRGM_Pos, 2>;
	using PaddingAlignmentMode = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_PAM_Pos, 2>;
	using PackEnable = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_PKE_Pos, 1>;
	using TransferLength = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_TLEN_Pos, 7>;
	using DstBurst = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_DBURST_Pos, 3>;
	using SrcBurst = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_SBURST_Pos, 3>;
	enum Sizes { Byte = 0b00, HalfWord = 0b01, Word = 0b10, DoubleWord = 0b11 };
	using DstIncAmount = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_DINCOS_Pos, 2>;
	using SrcIncAmount = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_SINCOS_Pos, 2>;
	using DstSize = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_DSIZE_Pos, 2>;
	using SrcSize = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_SSIZE_Pos, 2>;
	enum Directions { DoNotInc = 0b00, Up = 0b10, Down = 0b11 };
	using DstIncDir = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_DINC_Pos, 2>;
	using SrcIncDir = RegisterSection<ReadWrite, CTCR_Base, MDMA_CTCR_SINC_Pos, 2>;

	static constexpr uint32_t CBNDTR_Base = BASE + offsetof(MDMA_Channel_TypeDef, CBNDTR);
	using BlockRepeatCount = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BRC_Pos, 12>;
	enum BRUpdateMode { AddDestUpdateVal = 0b00, SubtractDestUpdateVal = 0b01 };
	using BlockRepeatDstUpdateMode = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BRDUM_Pos, 1>;
	using BlockRepeatSrcUpdateMode = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BRSUM_Pos, 1>;
	using BlockNumDataBytesToXfer = RegisterSection<ReadWrite, CBNDTR_Base, MDMA_CBNDTR_BNDT_Pos, 17>;

	static constexpr uint32_t CTBR_Base = BASE + offsetof(MDMA_Channel_TypeDef, CTBR);
	using DstBusIsAHBTCM = RegisterSection<ReadWrite, CTBR_Base, MDMA_CTBR_DBUS_Pos, 1>;
	using SrcBusIsAHBTCM = RegisterSection<ReadWrite, CTBR_Base, MDMA_CTBR_SBUS_Pos, 1>;
	using TriggerSelection = RegisterSection<ReadWrite, CTBR_Base, MDMA_CTBR_TSEL_Pos, 6>;

	using DstAddr = RegisterBits<ReadWrite, BASE + offsetof(MDMA_Channel_TypeDef, CDAR), 0xFFFFFFFF>;
	using SrcAddr = RegisterBits<ReadWrite, BASE + offsetof(MDMA_Channel_TypeDef, CSAR), 0xFFFFFFFF>;

	// Todo: CBRUR
	// Todo: CLAR
	// Todo: CMAR
	// Todo: CMDR
};
} // namespace mdrivlib
