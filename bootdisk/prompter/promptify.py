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

Bpb = namedtuple('Bpb', (
    'bytes_per_sector', 'sectors_per_cluster', 'reserved_sectors', 'num_fats',
    'num_root_entries', 'total_sectors', 'media_descriptor', 'sectors_per_fat',
    'sectors_per_track', 'num_heads',
))

def decode_bpb(boot_sector):
    assert boot_sector[0x36:0x3b] == b"FAT12", "only FAT12 volumes supported"
    fields = struct.unpack("<HBHBHHBHHH", boot_sector[11:28])
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

    table = []
    for i in range(bpb.total_sectors):
        base = (i // 2) * 3
        if i % 2 == 0:
            w, = struct.unpack("<H", fat[base:base+2])
            table.append(w & 0xfff)
        else:
            w, = struct.unpack("<H", fat[base+1:base+3])
            table.append((w >> 4) & 0xfff)

    return table

with open("boot", "rb") as f:
    prompt_boot_sector = f.read()

disk_image = open(sys.argv[1], "rb")
boot_sector = read_block(disk_image, 0)
bpb = decode_bpb(boot_sector)
print(bpb)

print(read_fat(disk_image, bpb))
