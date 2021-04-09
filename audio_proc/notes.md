### Notes ###

poweron: ROM loads FSBL from sdcard
FSBL inits DDR, uart4
FSBL loads SSBL
SSBL inits DDR, ...what else?
SSBL loads bare-arm.uimg from sdcard into 0xC2000040, executes

main app starts running on A7
[loads m4 firmware?]

|------------------------------------|---------------------------------------|
| A7                                 |   m4                                  |
|------------------------------------|---------------------------------------|
| sends "ready" to m4 (HSEM chan 0)  |                                       |
|                                    | gets "ready", starts codec, ADC DMA   |
|                                    | RX->mem1, TX<-dummy                   |
|                                    | codec RX/ADC data is ready, sends HSEM:1 |
| Gets HSEM:1, processes mem1->mem3  | RX->mem2, TX<-dummy                   |
| processing done, send HSEM:3       |                                       |
|                                    | Got HSEM:3, setup TX<-mem3            |
|                                    | codec RX/ADC data is ready, sends HSEM:2 |
| Gets HSEM:2, processes mem2->mem4  | RX->mem1, TX<-mem3                   |
| processing done, send HSEM:4       |                                       |
|                                    | Got HSEM:4, setup TX<-mem4            |
|                                    | codec RX/ADC data is ready, sends HSEM:1 |
| Gets HSEM:1, processes mem1->mem3  | RX->mem2, TX<-mem4                   |
| processing done, send HSEM:3       |                                       |
|                                    | Got HSEM:3, setup TX<-mem3            |
|                                    | codec RX/ADC data is ready, sends HSEM:2 |
| Gets HSEM:2, processes mem2->mem4  | RX->mem1, TX<-mem3                   |
| processing done, send HSEM:4       |                                       |
|                                    | Got HSEM:4, setup TX<-mem4            |
|                                    | codec RX/ADC data is ready, sends HSEM:1 |
...

HSEM:3/4 not needed after the first one.




