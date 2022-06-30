#define PAD_COUNT(s, pad) (((s) - 1) / (pad) + 1)
#define BLOCK_CNT(size, blk_desc) (PAD_COUNT(size, blk_desc->blksz))
