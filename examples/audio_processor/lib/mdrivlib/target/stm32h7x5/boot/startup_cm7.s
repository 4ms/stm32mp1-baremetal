/**
  ******************************************************************************
  * @file      startup_stm32h755xx.s
  * @author    MCD Application Team, amended by Dan Green danngreen1@gmail.com
  * @brief     Generic Devices vector table for use with mdrivlib
  *            This module performs:
  *                - Set the initial SP
  *                - Set the initial PC == Reset_Handler,
  *                - Set the vector table entries with the exceptions ISR address
  *                - Branches to main in the C library (which eventually
  *                  calls main()).
  *            After Reset the Cortex-M processor is in Thread mode,
  *            priority is Privileged, and the Stack is set to Main.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

  .syntax unified
  .cpu cortex-m7
  .fpu softvfp
  .thumb

.global  g_pfnVectors
.global  Default_Handler

/* start address for the initialization values of the .data section. 
defined in linker script */
.word  _sidata
/* start address for the .data section. defined in linker script */  
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss
/* stack used for SystemInit_ExtMemCtl; always internal RAM used */

    .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:  
  ldr   sp, =_estack

  bl  SystemInit

  movs  r1, #0
  b  LoopCopyDataInit

CopyDataInit:
  ldr  r3, =_sidata
  ldr  r3, [r3, r1]
  str  r3, [r0, r1]
  adds  r1, r1, #4

LoopCopyDataInit:
  ldr  r0, =_sdata
  ldr  r3, =_edata
  adds  r2, r0, r1
  cmp  r2, r3
  bcc  CopyDataInit
  ldr  r2, =_sbss
  b  LoopFillZerobss

FillZerobss:
  movs  r3, #0
  str  r3, [r2], #4

LoopFillZerobss:
  ldr  r3, = _ebss
  cmp  r2, r3
  bcc  FillZerobss

  bl __libc_init_array
  bl  main
  bx  lr    
.size  Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an 
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 * @param  None     
 * @retval None       
*/
    .section  .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b  Infinite_Loop
  .size  Default_Handler, .-Default_Handler
/******************************************************************************
*
* The minimal vector table for a Cortex M. Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
* 
*******************************************************************************/
   .section  .isr_vector,"a",%progbits
  .type  g_pfnVectors, %object
  .size  g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word  _estack
  .word  Reset_Handler

  .word  NMI_Handler
  .word  HardFault_Handler
  .word  MemManage_Handler
  .word  BusFault_Handler
  .word  UsageFault_Handler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  SVC_Handler
  .word  DebugMon_Handler
  .word  0
  .word  PendSV_Handler
  .word  SysTick_Handler

  /* External Interrupts: 
     C-linkage functions jump to
	 a C++ callable-object such as a lambda
	 See drivers/interrupt.hh
 */
  .word IRQ_Trampoline_0
  .word IRQ_Trampoline_1
  .word IRQ_Trampoline_2
  .word IRQ_Trampoline_3
  .word IRQ_Trampoline_4
  .word IRQ_Trampoline_5
  .word IRQ_Trampoline_6
  .word IRQ_Trampoline_7
  .word IRQ_Trampoline_8
  .word IRQ_Trampoline_9
  .word IRQ_Trampoline_10
  .word IRQ_Trampoline_11
  .word IRQ_Trampoline_12
  .word IRQ_Trampoline_13
  .word IRQ_Trampoline_14
  .word IRQ_Trampoline_15
  .word IRQ_Trampoline_16
  .word IRQ_Trampoline_17
  .word IRQ_Trampoline_18
  .word IRQ_Trampoline_19
  .word IRQ_Trampoline_20
  .word IRQ_Trampoline_21
  .word IRQ_Trampoline_22
  .word IRQ_Trampoline_23
  .word IRQ_Trampoline_24
  .word IRQ_Trampoline_25
  .word IRQ_Trampoline_26
  .word IRQ_Trampoline_27
  .word IRQ_Trampoline_28
  .word IRQ_Trampoline_29
  .word IRQ_Trampoline_30
  .word IRQ_Trampoline_31
  .word IRQ_Trampoline_32
  .word IRQ_Trampoline_33
  .word IRQ_Trampoline_34
  .word IRQ_Trampoline_35
  .word IRQ_Trampoline_36
  .word IRQ_Trampoline_37
  .word IRQ_Trampoline_38
  .word IRQ_Trampoline_39
  .word IRQ_Trampoline_40
  .word IRQ_Trampoline_41
  .word IRQ_Trampoline_42
  .word IRQ_Trampoline_43
  .word IRQ_Trampoline_44
  .word IRQ_Trampoline_45
  .word IRQ_Trampoline_46
  .word IRQ_Trampoline_47
  .word IRQ_Trampoline_48
  .word IRQ_Trampoline_49
  .word IRQ_Trampoline_50
  .word IRQ_Trampoline_51
  .word IRQ_Trampoline_52
  .word IRQ_Trampoline_53
  .word IRQ_Trampoline_54
  .word IRQ_Trampoline_55
  .word IRQ_Trampoline_56
  .word IRQ_Trampoline_57
  .word IRQ_Trampoline_58
  .word IRQ_Trampoline_59
  .word IRQ_Trampoline_60
  .word IRQ_Trampoline_61
  .word IRQ_Trampoline_62
  .word IRQ_Trampoline_63
  .word IRQ_Trampoline_64
  .word IRQ_Trampoline_65
  .word IRQ_Trampoline_66
  .word IRQ_Trampoline_67
  .word IRQ_Trampoline_68
  .word IRQ_Trampoline_69
  .word IRQ_Trampoline_70
  .word IRQ_Trampoline_71
  .word IRQ_Trampoline_72
  .word IRQ_Trampoline_73
  .word IRQ_Trampoline_74
  .word IRQ_Trampoline_75
  .word IRQ_Trampoline_76
  .word IRQ_Trampoline_77
  .word IRQ_Trampoline_78
  .word IRQ_Trampoline_79
  .word IRQ_Trampoline_80
  .word IRQ_Trampoline_81
  .word IRQ_Trampoline_82
  .word IRQ_Trampoline_83
  .word IRQ_Trampoline_84
  .word IRQ_Trampoline_85
  .word IRQ_Trampoline_86
  .word IRQ_Trampoline_87
  .word IRQ_Trampoline_88
  .word IRQ_Trampoline_89
  .word IRQ_Trampoline_90
  .word IRQ_Trampoline_91
  .word IRQ_Trampoline_92
  .word IRQ_Trampoline_93
  .word IRQ_Trampoline_94
  .word IRQ_Trampoline_95
  .word IRQ_Trampoline_96
  .word IRQ_Trampoline_97
  .word IRQ_Trampoline_98
  .word IRQ_Trampoline_99
  .word IRQ_Trampoline_100
  .word IRQ_Trampoline_101
  .word IRQ_Trampoline_102
  .word IRQ_Trampoline_103
  .word IRQ_Trampoline_104
  .word IRQ_Trampoline_105
  .word IRQ_Trampoline_106
  .word IRQ_Trampoline_107
  .word IRQ_Trampoline_108
  .word IRQ_Trampoline_109
  .word IRQ_Trampoline_110
  .word IRQ_Trampoline_111
  .word IRQ_Trampoline_112
  .word IRQ_Trampoline_113
  .word IRQ_Trampoline_114
  .word IRQ_Trampoline_115
  .word IRQ_Trampoline_116
  .word IRQ_Trampoline_117
  .word IRQ_Trampoline_118
  .word IRQ_Trampoline_119
  .word IRQ_Trampoline_120
  .word IRQ_Trampoline_121
  .word IRQ_Trampoline_122
  .word IRQ_Trampoline_123
  .word IRQ_Trampoline_124
  .word IRQ_Trampoline_125
  .word IRQ_Trampoline_126
  .word IRQ_Trampoline_127
  .word IRQ_Trampoline_128
  .word IRQ_Trampoline_129
  .word IRQ_Trampoline_130
  .word IRQ_Trampoline_131
  .word IRQ_Trampoline_132
  .word IRQ_Trampoline_133
  .word IRQ_Trampoline_134
  .word IRQ_Trampoline_135
  .word IRQ_Trampoline_136
  .word IRQ_Trampoline_137
  .word IRQ_Trampoline_138
  .word IRQ_Trampoline_139
  .word IRQ_Trampoline_140
  .word IRQ_Trampoline_141
  .word IRQ_Trampoline_142
  .word IRQ_Trampoline_143
  .word IRQ_Trampoline_144
  .word IRQ_Trampoline_145
  .word IRQ_Trampoline_146
  .word IRQ_Trampoline_147
  .word IRQ_Trampoline_148
  .word IRQ_Trampoline_149
  .word IRQ_Trampoline_150
  .word IRQ_Trampoline_151
  .word IRQ_Trampoline_152
  .word IRQ_Trampoline_153
  .word IRQ_Trampoline_154
  .word IRQ_Trampoline_155
  .word IRQ_Trampoline_156
  .word IRQ_Trampoline_157
  .word IRQ_Trampoline_158
  .word IRQ_Trampoline_159
  .word IRQ_Trampoline_160
  .word IRQ_Trampoline_161
  .word IRQ_Trampoline_162
  .word IRQ_Trampoline_163
  .word IRQ_Trampoline_164
  .word IRQ_Trampoline_165
  .word IRQ_Trampoline_166
  .word IRQ_Trampoline_167
  .word IRQ_Trampoline_168
  .word IRQ_Trampoline_169
  .word IRQ_Trampoline_170
  .word IRQ_Trampoline_171
  .word IRQ_Trampoline_172
  .word IRQ_Trampoline_173
  .word IRQ_Trampoline_174
  .word IRQ_Trampoline_175
  .word IRQ_Trampoline_176
  .word IRQ_Trampoline_177
  .word IRQ_Trampoline_178
  .word IRQ_Trampoline_179
  .word IRQ_Trampoline_180
  .word IRQ_Trampoline_181
  .word IRQ_Trampoline_182
  .word IRQ_Trampoline_183
  .word IRQ_Trampoline_184
  .word IRQ_Trampoline_185
  .word IRQ_Trampoline_186
  .word IRQ_Trampoline_187
  .word IRQ_Trampoline_188
  .word IRQ_Trampoline_189
  .word IRQ_Trampoline_190
  .word IRQ_Trampoline_191
  .word IRQ_Trampoline_192
  .word IRQ_Trampoline_193
  .word IRQ_Trampoline_194
  .word IRQ_Trampoline_195
  .word IRQ_Trampoline_196
  .word IRQ_Trampoline_197
  .word IRQ_Trampoline_198
  .word IRQ_Trampoline_199
  .word IRQ_Trampoline_200
  .word IRQ_Trampoline_201
  .word IRQ_Trampoline_202
  .word IRQ_Trampoline_203
  .word IRQ_Trampoline_204
  .word IRQ_Trampoline_205
  .word IRQ_Trampoline_206
  .word IRQ_Trampoline_207
  .word IRQ_Trampoline_208
  .word IRQ_Trampoline_209
  .word IRQ_Trampoline_210
  .word IRQ_Trampoline_211
  .word IRQ_Trampoline_212
  .word IRQ_Trampoline_213
  .word IRQ_Trampoline_214
  .word IRQ_Trampoline_215
  .word IRQ_Trampoline_216
  .word IRQ_Trampoline_217
  .word IRQ_Trampoline_218
  .word IRQ_Trampoline_219
  .word IRQ_Trampoline_220
  .word IRQ_Trampoline_221
  .word IRQ_Trampoline_222
  .word IRQ_Trampoline_223
  .word IRQ_Trampoline_224
  .word IRQ_Trampoline_225
  .word IRQ_Trampoline_226
  .word IRQ_Trampoline_227
  .word IRQ_Trampoline_228
  .word IRQ_Trampoline_229
  .word IRQ_Trampoline_230
  .word IRQ_Trampoline_231
  .word IRQ_Trampoline_232
  .word IRQ_Trampoline_233
  .word IRQ_Trampoline_234
  .word IRQ_Trampoline_235
  .word IRQ_Trampoline_236
  .word IRQ_Trampoline_237
  .word IRQ_Trampoline_238
  .word IRQ_Trampoline_239
  .word IRQ_Trampoline_240
  .word IRQ_Trampoline_241
  .word IRQ_Trampoline_242
  .word IRQ_Trampoline_243
  .word IRQ_Trampoline_244
  .word IRQ_Trampoline_245
  .word IRQ_Trampoline_246
  .word IRQ_Trampoline_247
  .word IRQ_Trampoline_248
  .word IRQ_Trampoline_249
  .word IRQ_Trampoline_250
  .word IRQ_Trampoline_251
  .word IRQ_Trampoline_252
  .word IRQ_Trampoline_253
  .word IRQ_Trampoline_254
  .word IRQ_Trampoline_255
