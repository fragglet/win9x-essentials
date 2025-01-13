#!/usr/bin/env python3
#
# Copyright (c) 2025 Simon Howard
#
# Permission to use, copy, modify, and/or distribute this software
# for any purpose with or without fee is hereby granted, provided
# that the above copyright notice and this permission notice appear
# in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
# WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
# AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
#
# This Python program installs the intermediate boot sector onto a floppy disk
# image that prompts the user to press enter, loading the real boot sector if
# they do.

from collections import namedtuple
import struct
import sys

SECTOR_SIZE = 512

FAT12_BAD_SECTOR = 0xff7

BOOT_HEADER_AREA_START = 0x03
BOOT_HEADER_AREA_END = 0x3c
BPB_OFFSET_START = 0x0b
BPB_OFFSET_END = 0x1c

BOOT_OFFSET_CHS = (512 - 5)

Bpb = namedtuple('Bpb', (
    'bytes_per_sector', 'sectors_per_cluster', 'reserved_sectors', 'num_fats',
    'num_root_entries', 'total_sectors', 'media_descriptor', 'sectors_per_fat',
    'sectors_per_track', 'num_heads',
))

def decode_bpb(boot_sector):
    assert boot_sector[0x36:0x3b] == b"FAT12", "only FAT12 volumes supported"
    bpb = boot_sector[BPB_OFFSET_START:BPB_OFFSET_END]
    fields = struct.unpack("<HBHBHHBHHH", bpb)
    result = Bpb(*fields)
    assert result.bytes_per_sector == SECTOR_SIZE, (
        "want %d bytes per sector, got %d in existing boot sector" % (
            SECTOR_SIZE, result.bytes_per_sector,
        ))
    assert result.sectors_per_cluster == 1, (
        "only one sector per cluster supported, got %d" % (
            results.sectors_per_cluster,
        ))
    return result

def read_block(f, block_num):
    f.seek(block_num * SECTOR_SIZE)
    result = f.read(SECTOR_SIZE)
    assert(len(result) == SECTOR_SIZE)
    return result

def write_blocks(f, block_num, block):
    assert(len(block) % SECTOR_SIZE == 0)
    f.seek(block_num * SECTOR_SIZE)
    f.write(block)

def leading_sectors(bpb):
    """Returns number of sectors before first data sector"""
    return (bpb.reserved_sectors + bpb.num_fats * bpb.sectors_per_fat
          + bpb.num_root_entries // 16)

def read_fat(f, bpb):
    first_fat = None
    for i in range(bpb.num_fats):
        fat_offset = bpb.reserved_sectors + i * bpb.sectors_per_fat
        blocks = []
        for b in range(bpb.sectors_per_fat):
            blocks.append(read_block(f, fat_offset + b))
        fat = b''.join(blocks)
        assert i == 0 or fat == first_fat, ("FAT %d does not match FAT 0" % i)
        first_fat = fat

    # Unmarshal to FAT array. The table has two special leading entries.
    num_clusters = bpb.total_sectors - leading_sectors(bpb)
    table = []
    for i in range(num_clusters + 2):
        base = (i // 2) * 3
        if i % 2 == 0:
            w, = struct.unpack("<H", fat[base:base+2])
            table.append(w & 0xfff)
        else:
            w, = struct.unpack("<H", fat[base+1:base+3])
            table.append((w >> 4) & 0xfff)

    return table

def write_fat(f, bpb, fat):
    fat += [0]
    chunks = []
    for i in range(len(fat) // 2):
        val = fat[i * 2] | (fat[i * 2 + 1] << 12)
        chunks.append(struct.pack("<L", val)[:3])
    fat_data = b"".join(chunks)
    fat_data += b"\0" * (SECTOR_SIZE * bpb.sectors_per_fat - len(fat_data))

    for i in range(bpb.num_fats):
        offset = bpb.reserved_sectors + i * bpb.sectors_per_fat
        write_blocks(f, offset,fat_data)

def to_chs(sectors_per_track, block_num):
    return (
        block_num // (2 * sectors_per_track),  # cylinder
        (block_num // sectors_per_track) % 2, # head (side)
        (block_num % sectors_per_track) + 1 # sector
    )

with open("boot", "rb") as f:
    prompt_boot_sector = f.read()

disk_image = open(sys.argv[1], "rb+")
boot_sector = read_block(disk_image, 0)
bpb = decode_bpb(boot_sector)
print(bpb)

fat = read_fat(disk_image, bpb)

# Which sector are we going to patch?
for c in reversed(range(len(fat))):
    if fat[c] == 0:
        patch_cluster = c
        break
else:
    assert False, "Failed to find a free sector for the real boot sector"

patch_sector = (patch_cluster - 2) + leading_sectors(bpb)
patch_c, patch_h, patch_s = to_chs(bpb.sectors_per_track, patch_sector)

# Save the old boot sector:
write_blocks(disk_image, patch_sector, boot_sector)
print("Saved old boot sector to block #%d (CHS: %d/%d/%d)" % (
    patch_sector, patch_c, patch_h, patch_s))

# Update the FAT, mark the old boot sector as a bad sector:
fat[patch_cluster] = FAT12_BAD_SECTOR
write_fat(disk_image, bpb, fat)

# Now we can construct the replacement boot sector. We copy the FAT header from
# the real boot sector and bake in the C/H/S address of the original:
prompt_boot_sector = bytearray(prompt_boot_sector)
prompt_boot_sector[BOOT_HEADER_AREA_START:BOOT_HEADER_AREA_END] = (
    boot_sector[BOOT_HEADER_AREA_START:BOOT_HEADER_AREA_END])
prompt_boot_sector[BOOT_OFFSET_CHS:BOOT_OFFSET_CHS + 3] = (
    patch_c, patch_h, patch_s)
write_blocks(disk_image, 0, prompt_boot_sector)
