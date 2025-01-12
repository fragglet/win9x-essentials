#!/usr/bin/env python3

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
    fields = struct.unpack("<HBHBHHBHHH", boot_sector[11:28])
    result = Bpb(*fields)
    assert result.bytes_per_sector == SECTOR_SIZE, (
        "want %d bytes per sector, got %d in existing boot sector" % (
            SECTOR_SIZE, result.bytes_per_sector,
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
    return first_fat

with open("boot", "rb") as f:
    prompt_boot_sector = f.read()

disk_image = open(sys.argv[1], "rb")
boot_sector = read_block(disk_image, 0)
bpb = decode_bpb(boot_sector)
print(bpb)

print(read_fat(disk_image, bpb))
