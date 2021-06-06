// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 */

#include <common.h>
#include <cbfs.h>
#include <malloc.h>
#include <asm/byteorder.h>

static const u32 good_magic = 0x4f524243;
static const u8 good_file_magic[] = "LARCHIVE";

struct cbfs_priv {
	int initialized;
	struct cbfs_header header;
	struct cbfs_cachenode *file_cache;
	enum cbfs_result result;
};

static struct cbfs_priv cbfs_s;

const char *file_cbfs_error(void)
{
	switch (cbfs_s.result) {
	case CBFS_SUCCESS:
		return "Success";
	case CBFS_NOT_INITIALIZED:
		return "CBFS not initialized";
	case CBFS_BAD_HEADER:
		return "Bad CBFS header";
	case CBFS_BAD_FILE:
		return "Bad CBFS file";
	case CBFS_FILE_NOT_FOUND:
		return "File not found";
	default:
		return "Unknown";
	}
}

enum cbfs_result cbfs_get_result(void)
{
	return cbfs_s.result;
}

/* Do endian conversion on the CBFS header structure. */
static void swap_header(struct cbfs_header *dest, struct cbfs_header *src)
{
	dest->magic = be32_to_cpu(src->magic);
	dest->version = be32_to_cpu(src->version);
	dest->rom_size = be32_to_cpu(src->rom_size);
	dest->boot_block_size = be32_to_cpu(src->boot_block_size);
	dest->align = be32_to_cpu(src->align);
	dest->offset = be32_to_cpu(src->offset);
}

/* Do endian conversion on a CBFS file header. */
static void swap_file_header(struct cbfs_fileheader *dest,
			     const struct cbfs_fileheader *src)
{
	memcpy(&dest->magic, &src->magic, sizeof(dest->magic));
	dest->len = be32_to_cpu(src->len);
	dest->type = be32_to_cpu(src->type);
	dest->attributes_offset = be32_to_cpu(src->attributes_offset);
	dest->offset = be32_to_cpu(src->offset);
}

/*
 * Given a starting position in memory, scan forward, bounded by a size, and
 * find the next valid CBFS file. No memory is allocated by this function. The
 * caller is responsible for allocating space for the new file structure.
 *
 * @param start		The location in memory to start from.
 * @param size		The size of the memory region to search.
 * @param align		The alignment boundaries to check on.
 * @param new_node	A pointer to the file structure to load.
 * @param used		A pointer to the count of of bytes scanned through,
 *			including the file if one is found.
 *
 * @return 1 if a file is found, 0 if one isn't.
 */
static int file_cbfs_next_file(struct cbfs_priv *priv, u8 *start, u32 size,
			       u32 align, struct cbfs_cachenode *new_node,
			       u32 *used)
{
	struct cbfs_fileheader header;

	*used = 0;

	while (size >= align) {
		const struct cbfs_fileheader *file_header =
			(const struct cbfs_fileheader *)start;
		u32 name_len;
		u32 step;

		/* Check if there's a file here. */
		if (memcmp(good_file_magic, &file_header->magic,
			   sizeof(file_header->magic))) {
			*used += align;
			size -= align;
			start += align;
			continue;
		}

		swap_file_header(&header, file_header);
		if (header.offset < sizeof(struct cbfs_fileheader)) {
			priv->result = CBFS_BAD_FILE;
			return -1;
		}
		new_node->next = NULL;
		new_node->type = header.type;
		new_node->data = start + header.offset;
		new_node->data_length = header.len;
		name_len = header.offset - sizeof(struct cbfs_fileheader);
		new_node->name = (char *)file_header +
				sizeof(struct cbfs_fileheader);
		new_node->name_length = name_len;
		new_node->attributes_offset = header.attributes_offset;

		step = header.len;
		if (step % align)
			step = step + align - step % align;

		*used += step;
		return 1;
	}
	return 0;
}

/* Look through a CBFS instance and copy file metadata into regular memory. */
static void file_cbfs_fill_cache(struct cbfs_priv *priv, u8 *start, u32 size,
				 u32 align)
{
	struct cbfs_cachenode *cache_node;
	struct cbfs_cachenode *new_node;
	struct cbfs_cachenode **cache_tail = &priv->file_cache;

	/* Clear out old information. */
	cache_node = priv->file_cache;
	while (cache_node) {
		struct cbfs_cachenode *old_node = cache_node;
		cache_node = cache_node->next;
		free(old_node);
	}
	priv->file_cache = NULL;

	while (size >= align) {
		int result;
		u32 used;

		new_node = (struct cbfs_cachenode *)
				malloc(sizeof(struct cbfs_cachenode));
		result = file_cbfs_next_file(priv, start, size, align, new_node,
					     &used);

		if (result < 0) {
			free(new_node);
			return;
		} else if (result == 0) {
			free(new_node);
			break;
		}
		*cache_tail = new_node;
		cache_tail = &new_node->next;

		size -= used;
		start += used;
	}
	priv->result = CBFS_SUCCESS;
}

/* Get the CBFS header out of the ROM and do endian conversion. */
static int file_cbfs_load_header(uintptr_t end_of_rom,
				 struct cbfs_header *header)
{
	struct cbfs_header *header_in_rom;
	int32_t offset = *(u32 *)(end_of_rom - 3);

	header_in_rom = (struct cbfs_header *)(end_of_rom + offset + 1);
	swap_header(header, header_in_rom);

	if (header->magic != good_magic || header->offset >
			header->rom_size - header->boot_block_size) {
		cbfs_s.result = CBFS_BAD_HEADER;
		return 1;
	}
	return 0;
}

static int cbfs_load_header_ptr(struct cbfs_priv *priv, ulong base,
				struct cbfs_header *header)
{
	struct cbfs_header *header_in_rom;

	header_in_rom = (struct cbfs_header *)base;
	swap_header(header, header_in_rom);

	if (header->magic != good_magic || header->offset >
			header->rom_size - header->boot_block_size) {
		priv->result = CBFS_BAD_HEADER;
		return -EFAULT;
	}

	return 0;
}

static void cbfs_init(struct cbfs_priv *priv, uintptr_t end_of_rom)
{
	u8 *start_of_rom;

	priv->initialized = 0;

	if (file_cbfs_load_header(end_of_rom, &priv->header))
		return;

	start_of_rom = (u8 *)(end_of_rom + 1 - priv->header.rom_size);

	file_cbfs_fill_cache(priv, start_of_rom, priv->header.rom_size,
			     priv->header.align);
	if (priv->result == CBFS_SUCCESS)
		priv->initialized = 1;
}

void file_cbfs_init(uintptr_t end_of_rom)
{
	cbfs_init(&cbfs_s, end_of_rom);
}

int cbfs_init_mem(ulong base, ulong size, struct cbfs_priv **privp)
{
	struct cbfs_priv priv_s, *priv = &priv_s;
	int ret;

	/*
	 * Use a local variable to start with until we know that the CBFS is
	 * valid. Assume that a master header appears at the start, at offset
	 * 0x38.
	 */
	ret = cbfs_load_header_ptr(priv, base + 0x38, &priv->header);
	if (ret)
		return ret;

	file_cbfs_fill_cache(priv, (u8 *)base, priv->header.rom_size,
			     priv->header.align);
	if (priv->result != CBFS_SUCCESS)
		return -EINVAL;

	priv->initialized = 1;
	priv = malloc(sizeof(priv_s));
	if (!priv)
		return -ENOMEM;
	memcpy(priv, &priv_s, sizeof(priv_s));
	*privp = priv;

	return 0;
}

const struct cbfs_header *file_cbfs_get_header(void)
{
	struct cbfs_priv *priv = &cbfs_s;

	if (priv->initialized) {
		priv->result = CBFS_SUCCESS;
		return &priv->header;
	} else {
		priv->result = CBFS_NOT_INITIALIZED;
		return NULL;
	}
}

const struct cbfs_cachenode *file_cbfs_get_first(void)
{
	struct cbfs_priv *priv = &cbfs_s;

	if (!priv->initialized) {
		priv->result = CBFS_NOT_INITIALIZED;
		return NULL;
	} else {
		priv->result = CBFS_SUCCESS;
		return priv->file_cache;
	}
}

void file_cbfs_get_next(const struct cbfs_cachenode **file)
{
	struct cbfs_priv *priv = &cbfs_s;

	if (!priv->initialized) {
		priv->result = CBFS_NOT_INITIALIZED;
		*file = NULL;
		return;
	}

	if (*file)
		*file = (*file)->next;
	priv->result = CBFS_SUCCESS;
}

const struct cbfs_cachenode *cbfs_find_file(struct cbfs_priv *priv,
					    const char *name)
{
	struct cbfs_cachenode *cache_node = priv->file_cache;

	if (!priv->initialized) {
		priv->result = CBFS_NOT_INITIALIZED;
		return NULL;
	}

	while (cache_node) {
		if (!strcmp(name, cache_node->name))
			break;
		cache_node = cache_node->next;
	}
	if (!cache_node)
		priv->result = CBFS_FILE_NOT_FOUND;
	else
		priv->result = CBFS_SUCCESS;

	return cache_node;
}

const struct cbfs_cachenode *file_cbfs_find(const char *name)
{
	return cbfs_find_file(&cbfs_s, name);
}

const struct cbfs_cachenode *file_cbfs_find_uncached(uintptr_t end_of_rom,
						     const char *name)
{
	struct cbfs_priv *priv = &cbfs_s;
	u8 *start;
	u32 size;
	u32 align;
	static struct cbfs_cachenode node;

	if (file_cbfs_load_header(end_of_rom, &priv->header))
		return NULL;

	start = (u8 *)(end_of_rom + 1 - priv->header.rom_size);
	size = priv->header.rom_size;
	align = priv->header.align;

	while (size >= align) {
		int result;
		u32 used;

		result = file_cbfs_next_file(priv, start, size, align, &node,
					     &used);

		if (result < 0)
			return NULL;
		else if (result == 0)
			break;

		if (!strcmp(name, node.name))
			return &node;

		size -= used;
		start += used;
	}
	cbfs_s.result = CBFS_FILE_NOT_FOUND;
	return NULL;
}

const char *file_cbfs_name(const struct cbfs_cachenode *file)
{
	cbfs_s.result = CBFS_SUCCESS;

	return file->name;
}

u32 file_cbfs_size(const struct cbfs_cachenode *file)
{
	cbfs_s.result = CBFS_SUCCESS;

	return file->data_length;
}

u32 file_cbfs_type(const struct cbfs_cachenode *file)
{
	cbfs_s.result = CBFS_SUCCESS;

	return file->type;
}

long file_cbfs_read(const struct cbfs_cachenode *file, void *buffer,
		    unsigned long maxsize)
{
	u32 size;

	size = file->data_length;
	if (maxsize && size > maxsize)
		size = maxsize;

	memcpy(buffer, file->data, size);
	cbfs_s.result = CBFS_SUCCESS;

	return size;
}
