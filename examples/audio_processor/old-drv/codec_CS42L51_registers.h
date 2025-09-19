#pragma once
#include <cstdint>

namespace mdrivlib::CodecCS42L51Register
{
constexpr uint8_t CHIP_ID = 0x1B;
constexpr uint8_t CHIP_REV_A = 0x00;
constexpr uint8_t CHIP_REV_B = 0x01;
constexpr uint8_t CHIP_REV_MASK = 0x07;

constexpr uint8_t CHIP_REV_ID = 0x01;
constexpr uint8_t MK_CHIP_REV(uint8_t a, uint8_t b) {
	return a << 3 | b;
}

constexpr uint8_t POWER_CTL1 = 0x02;
constexpr uint8_t POWER_CTL1_PDN_DACB = (1 << 6);
constexpr uint8_t POWER_CTL1_PDN_DACA = (1 << 5);
constexpr uint8_t POWER_CTL1_PDN_PGAB = (1 << 4);
constexpr uint8_t POWER_CTL1_PDN_PGAA = (1 << 3);
constexpr uint8_t POWER_CTL1_PDN_ADCB = (1 << 2);
constexpr uint8_t POWER_CTL1_PDN_ADCA = (1 << 1);
constexpr uint8_t POWER_CTL1_PDN = (1 << 0);

constexpr uint8_t MIC_POWER_CTL = 0x03;
constexpr uint8_t MIC_POWER_CTL_AUTO = (1 << 7);
constexpr uint8_t MIC_POWER_CTL_SPEED(uint8_t x) {
	return (x & 3) << 5;
}
constexpr uint8_t QSM_MODE = 3;
constexpr uint8_t HSM_MODE = 2;
constexpr uint8_t SSM_MODE = 1;
constexpr uint8_t DSM_MODE = 0;
constexpr uint8_t MIC_POWER_CTL_3ST_SP = (1 << 4);
constexpr uint8_t MIC_POWER_CTL_PDN_MICB = (1 << 3);
constexpr uint8_t MIC_POWER_CTL_PDN_MICA = (1 << 2);
constexpr uint8_t MIC_POWER_CTL_PDN_BIAS = (1 << 1);
constexpr uint8_t MIC_POWER_CTL_MCLK_DIV2 = (1 << 0);

constexpr uint8_t INTF_CTL = 0x04;
constexpr uint8_t INTF_CTL_LOOPBACK = (1 << 7);
constexpr uint8_t INTF_CTL_MASTER = (1 << 6);
constexpr uint8_t INTF_CTL_DAC_FORMAT(uint8_t x) {
	return (x & 7) << 3;
}
constexpr uint8_t DAC_DIF_LJ24 = 0x00;
constexpr uint8_t DAC_DIF_I2S = 0x01;
constexpr uint8_t DAC_DIF_RJ24 = 0x02;
constexpr uint8_t DAC_DIF_RJ20 = 0x03;
constexpr uint8_t DAC_DIF_RJ18 = 0x04;
constexpr uint8_t DAC_DIF_RJ16 = 0x05;
constexpr uint8_t INTF_CTL_ADC_I2S = (1 << 2);
constexpr uint8_t INTF_CTL_DIGMIX = (1 << 1);
constexpr uint8_t INTF_CTL_MICMIX = (1 << 0);

constexpr uint8_t MIC_CTL = 0x05;
constexpr uint8_t MIC_CTL_ADC_SNGVOL = (1 << 7);
constexpr uint8_t MIC_CTL_ADCD_DBOOST = (1 << 6);
constexpr uint8_t MIC_CTL_ADCA_DBOOST = (1 << 5);
constexpr uint8_t MIC_CTL_MICBIAS_SEL = (1 << 4);
constexpr uint8_t MIC_CTL_MICBIAS_LVL(uint8_t x) {
	return (x & 3) << 2;
}
constexpr uint8_t MIC_CTL_MICB_BOOST = (1 << 1);
constexpr uint8_t MIC_CTL_MICA_BOOST = (1 << 0);

constexpr uint8_t ADC_CTL = 0x06;
constexpr uint8_t ADC_CTL_ADCB_HPFEN = (1 << 7);
constexpr uint8_t ADC_CTL_ADCB_HPFRZ = (1 << 6);
constexpr uint8_t ADC_CTL_ADCA_HPFEN = (1 << 5);
constexpr uint8_t ADC_CTL_ADCA_HPFRZ = (1 << 4);
constexpr uint8_t ADC_CTL_SOFTB = (1 << 3);
constexpr uint8_t ADC_CTL_ZCROSSB = (1 << 2);
constexpr uint8_t ADC_CTL_SOFTA = (1 << 1);
constexpr uint8_t ADC_CTL_ZCROSSA = (1 << 0);

constexpr uint8_t ADC_INPUT = 0x07;
constexpr uint8_t ADC_INPUT_AINB_MUX(uint8_t x) {
	return (((x)&3) << 6);
}
constexpr uint8_t ADC_INPUT_AINA_MUX(uint8_t x) {
	return (((x)&3) << 4);
}
constexpr uint8_t MUX_AIN1 = 0b00;
constexpr uint8_t MUX_AIN2 = 0b01;
constexpr uint8_t MUX_AIN3_MIC = 0b10;
constexpr uint8_t MUX_INV_ADCB = (1 << 3);
constexpr uint8_t ADC_INPUT_INV_ADCA = (1 << 2);
constexpr uint8_t ADC_INPUT_ADCB_MUTE = (1 << 1);
constexpr uint8_t ADC_INPUT_ADCA_MUTE = (1 << 0);

constexpr uint8_t DAC_OUT_CTL = 0x08;
constexpr uint8_t DAC_OUT_CTL_HP_GAIN(uint8_t x) {
	return (((x)&7) << 5);
}
constexpr uint8_t DAC_OUT_CTL_DAC_SNGVOL = (1 << 4);
constexpr uint8_t DAC_OUT_CTL_INV_PCMB = (1 << 3);
constexpr uint8_t DAC_OUT_CTL_INV_PCMA = (1 << 2);
constexpr uint8_t DAC_OUT_CTL_DACB_MUTE = (1 << 1);
constexpr uint8_t DAC_OUT_CTL_DACA_MUTE = (1 << 0);

constexpr uint8_t DAC_CTL = 0x09;
constexpr uint8_t DAC_CTL_DATA_SEL(uint8_t x) {
	return (((x)&3) << 6);
}
constexpr uint8_t DAC_CTL_FREEZE = (1 << 5);
constexpr uint8_t DAC_CTL_DEEMPH = (1 << 3);
constexpr uint8_t DAC_CTL_AMUTE = (1 << 2);
constexpr uint8_t DAC_CTL_DACSZ(uint8_t x) {
	return (((x)&3) << 0);
}

constexpr uint8_t ALC_PGA_CTL = 0x0A;
constexpr uint8_t ALC_PGB_CTL = 0x0B;
constexpr uint8_t ALC_PGX_ALCX_SRDIS = (1 << 7);
constexpr uint8_t ALC_PGX_ALCX_ZCDIS = (1 << 6);
constexpr uint8_t ALC_PGX_PGX_VOL(uint8_t x) {
	return (((x)&0x1f) << 0);
}

constexpr uint8_t ADCA_ATT = 0x0C;
constexpr uint8_t ADCB_ATT = 0x0D;

constexpr uint8_t ADCA_VOL = 0x0E;
constexpr uint8_t ADCB_VOL = 0x0F;
constexpr uint8_t PCMA_VOL = 0x10;
constexpr uint8_t PCMB_VOL = 0x11;
constexpr uint8_t MIX_MUTE_ADCMIX = (1 << 7);
constexpr uint8_t MIX_VOLUME(uint8_t x) {
	return (((x)&0x7f) << 0);
}

constexpr uint8_t BEEP_FREQ = 0x12;
constexpr uint8_t BEEP_VOL = 0x13;
constexpr uint8_t BEEP_CONF = 0x14;

constexpr uint8_t TONE_CTL = 0x15;
constexpr uint8_t TONE_CTL_TREB(uint8_t x) {
	return (((x)&0xf) << 4);
}
constexpr uint8_t TONE_CTL_BASS(uint8_t x) {
	return (((x)&0xf) << 0);
}

constexpr uint8_t AOUTA_VOL = 0x16;
constexpr uint8_t AOUTB_VOL = 0x17;
constexpr uint8_t PCM_MIXER = 0x18;
constexpr uint8_t LIMIT_THRES_DIS = 0x19;
constexpr uint8_t LIMIT_REL = 0x1A;
constexpr uint8_t LIMIT_ATT = 0x1B;
constexpr uint8_t ALC_EN = 0x1C;
constexpr uint8_t ALC_REL = 0x1D;
constexpr uint8_t ALC_THRES = 0x1E;
constexpr uint8_t NOISE_CONF = 0x1F;

constexpr uint8_t STATUS = 0x20;
constexpr uint8_t STATUS_SP_CLKERR = (1 << 6);
constexpr uint8_t STATUS_SPEA_OVFL = (1 << 5);
constexpr uint8_t STATUS_SPEB_OVFL = (1 << 4);
constexpr uint8_t STATUS_PCMA_OVFL = (1 << 3);
constexpr uint8_t STATUS_PCMB_OVFL = (1 << 2);
constexpr uint8_t STATUS_ADCA_OVFL = (1 << 1);
constexpr uint8_t STATUS_ADCB_OVFL = (1 << 0);

constexpr uint8_t CHARGE_FREQ = 0x21;

} // namespace mdrivlib::CodecCS42L51Register
