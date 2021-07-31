#pragma once
#include <cstdint>

namespace mdrivlib::CodecWM8731Registers
{
#define WM8731_ADDR_0 0x1A
#define WM8731_ADDR_1 0x1B

#define WM8731_NUM_REGS 10

// Registers:
#define WM8731_REG_INBOTH ((uint8_t)8)
#define WM8731_REG_INMUTE ((uint8_t)7)
#define WM8731_REG_POWERDOWN ((uint8_t)6)
#define WM8731_REG_INVOL ((uint8_t)0)

#define WM8731_REG_SAMPLE_CTRL 0x08
#define WM8731_REG_RESET 0x0F

#define VOL_p12dB 0b11111 /*+12dB*/
#define VOL_0dB 0b10111	  /*0dB*/
#define VOL_n1dB 0b10110  /*-1.5dB*/
#define VOL_n3dB 0b10101  /*-3dB*/
#define VOL_n6dB 0b10011  /*-6dB*/
#define VOL_n12dB 15	  /*-12dB*/
#define VOL_n15dB 13	  /*-15dB*/
/*1.5dB steps down to..*/
#define VOL_n34dB 0b00000 /*-34.5dB*/

#define HPVOL_p6dB (0b1111111)
#define HPVOL_p5dB (0b1111110)
//...
#define HPVOL_0dB (0b1111001)

// Register 4: Analogue Audio Path Control
#define MICBOOST (1 << 0)	/* Boost Mic level */
#define MUTEMIC (1 << 1)	/* Mute Mic to ADC */
#define INSEL_mic (1 << 2)	/* Mic Select*/
#define INSEL_line (0 << 2) /* LineIn Select*/
#define BYPASS (1 << 3)		/* Bypass Enable */
#define DACSEL (1 << 4)		/* Select DAC */
#define SIDETONE (1 << 5)	/* Enable Sidetone */
#define SIDEATT_neg15dB (0b11 << 6)
#define SIDEATT_neg12dB (0b10 << 6)
#define SIDEATT_neg9dB (0b01 << 6)
#define SIDEATT_neg6dB (0b00 << 6)

// Register 5: Digital Audio Path Control
#define ADCHPFDisable 1 /* ADC High Pass Filter */
#define ADCHPFEnable 0
#define DEEMPH_48k (0b11 << 1) /* De-emphasis Control */
#define DEEMPH_44k (0b10 << 1)
#define DEEMPH_32k (0b01 << 1)
#define DEEMPH_disable (0b00 << 1)
#define DACMU_enable (1 << 3) /* DAC Soft Mute Control */
#define DACMU_disable (0 << 3)
#define HPOR_store (1 << 4) /* Store DC offset when HPF disabled */
#define HPOR_clear (0 << 4)

// Register 6: Power Down Control: 1= enable power down, 0=disable power down
#define PD_LINEIN (1 << 0)
#define PD_MIC (1 << 1)
#define PD_ADC (1 << 2)
#define PD_DAC (1 << 3)
#define PD_OUT (1 << 4)
#define PD_OSC (1 << 5)
#define PD_CLKOUT (1 << 6)
#define PD_POWEROFF (1 << 7)

// Register 7: Digital Audio Interface Format
#define format_MSB_Right 0
#define format_MSB_Left 1
#define format_I2S 2
#define format_DSP 3
#define format_16b (0 << 2)
#define format_20b (1 << 2)
#define format_24b (2 << 2)
#define format_32b (3 << 2)

// Register: Sample Rate Controls
#define SR_USB_NORM (1 << 0) // 1=USB (250/272fs), 0=Normal Mode (256/384fs)
#define SR_BOSR (1 << 1)	 // Base Over-Sampling Rate: 0=250/256fs, 1=272/384fs (also 128/192)
#define SR_NORM_8K (0b1011 << 2)
#define SR_NORM_32K (0b0110 << 2)
#define SR_NORM_44K (0b1000 << 2)
#define SR_NORM_48K (0b0000 << 2)
#define SR_NORM_88K (0b1111 << 2)
#define SR_NORM_96K (0b0111 << 2)

}; // namespace mdrivlib::CodecWM8731Registers
