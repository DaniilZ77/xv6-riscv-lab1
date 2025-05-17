#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// run: clang main.c -o getinode
//      or
//      gcc main.c -o getinode

uint32_t read4bytes(const void *buf, int need_swap) {
    uint32_t val;
    memcpy(&val, buf, sizeof(val));
    return need_swap ? __builtin_bswap32(val) : val;
}

uint16_t read2bytes(const void *buf, int need_swap) {
    uint16_t val;
    memcpy(&val, buf, sizeof(val));
    return need_swap ? __builtin_bswap16(val) : val;
}

uint64_t cov_size(int level, uint32_t bs) {
    uint64_t req_size = bs;
    for (int i = 0; i < level; ++i)
        req_size *= (bs / 4);
    return req_size;
}

void write_zeros(uint64_t n) {
    static char zeros[4096] = {0};
    while (n) {
        size_t chunk = n > sizeof(zeros) ? sizeof(zeros) : (size_t)n;
        fwrite(zeros, 1, chunk, stdout);
        n -= chunk;
    }
}

void out_blocks(FILE *img, uint32_t block_num, int level, uint32_t bs, int need_swap, uint64_t *remained) {
    if (*remained == 0)
        return;
    if (block_num == 0) {
        uint64_t req_size = cov_size(level, bs);
        uint64_t want = (*remained < req_size) ? *remained : req_size;
        write_zeros(want);
        *remained -= want;
        return;
    }

    if (level == 0) {
        char *data = malloc(bs);
        if (!data) {
            perror("malloc");
            exit(1);
        }
        if (fseek(img, (uint64_t)block_num * bs, SEEK_SET) != 0) {
            perror("fseek data block");
            exit(1);
        }
        if (fread(data, bs, 1, img) != 1) {
            perror("read block");
            exit(1);
        }
        size_t write = *remained < bs ? *remained : bs;
        fwrite(data, 1, write, stdout);
        free(data);
        *remained -= write;
    } else {
        char *data = malloc(bs);
        if (!data) {
            perror("malloc");
            exit(1);
        }
        if (fseek(img, (uint64_t)block_num * bs, SEEK_SET) != 0) {
            perror("fseek indirect block");
            exit(1);
        }
        if (fread(data, bs, 1, img) != 1) {
            perror("read indirect");
            exit(1);
        }
        for (int i = 0; i < bs / 4 && *remained > 0; i++) {
            out_blocks(img, read4bytes(data + i*4, need_swap), level-1, bs, need_swap, remained);
        }
        free(data);
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s image inode\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *end;
    uint32_t inode = strtoul(argv[2], &end, 10);
    if (*end || inode < 1) {
        fprintf(stderr, "invalid inode\n");
        return EXIT_FAILURE;
    }

    FILE *img = fopen(argv[1], "rb");
    if (!img) {
        perror(argv[1]);
        return EXIT_FAILURE;
    }

    uint32_t bs = 1024;
    int need_swap = 0;
    char super[1024];
    if (fseek(img, 1024, SEEK_SET) != 0) {
        perror("fseek superblock");
        fclose(img);
        return EXIT_FAILURE;
    }
    if (fread(super, 1, 1024, img) != 1024) {
        perror("read superblock");
        fclose(img);
        return EXIT_FAILURE;
    }
    if (read2bytes(super + 0x38, 0) != 0xEF53 && (need_swap = 1, read2bytes(super + 0x38, 1) != 0xEF53)) {
        fprintf(stderr, "bad magic\n");
        fclose(img);
        return EXIT_FAILURE;
    }
    bs = 1024 << read4bytes(super + 0x18, need_swap);
    uint32_t ipgroup = read4bytes(super + 0x28, need_swap);
    uint32_t group = (inode - 1) / ipgroup;
    uint32_t dpblock = bs / 32;
    uint32_t dblock = 1024 / bs + 1 + (group / dpblock);
    uint32_t doffset = (group % dpblock) * 32;
    char dsc[32];
    if (fseek(img, dblock * bs + doffset, SEEK_SET) != 0) {
        perror("fseek group dsc");
        fclose(img);
        return EXIT_FAILURE;
    }
    if (fread(dsc, 1, 32, img) != 32) {
        perror("read group dsc");
        fclose(img);
        return EXIT_FAILURE;
    }

    uint32_t itable = read4bytes(dsc + 0x08, need_swap);
    uint16_t isize = read2bytes(super + 0x58, need_swap);
    if (isize == 0) isize = 128;
    uint64_t ipos = (uint64_t)itable * bs + (uint64_t)((inode - 1) % ipgroup) * isize;
    char inode_buf[512];
    if (isize > sizeof(inode_buf)) {
        fprintf(stderr, "inode too big (%u)\n", isize);
        fclose(img);
        return EXIT_FAILURE;
    }
    if (fseek(img, ipos, SEEK_SET) || fread(inode_buf, 1, isize, img) != isize) {
        perror("read inode");
        fclose(img);
        return EXIT_FAILURE;
    }
    uint64_t file_size = ((uint64_t)read4bytes(inode_buf + 0x6C, need_swap) << 32) | read4bytes(inode_buf + 4, need_swap);
    uint32_t blocks[15];
    for (int i = 0; i < 15; i++) {
        blocks[i] = read4bytes(inode_buf + 0x28 + i * 4, need_swap);
    }
    for (int i = 0; i < 12 && file_size; i++)
        out_blocks(img, blocks[i], 0, bs, need_swap, &file_size);
    if (file_size)
        out_blocks(img, blocks[12], 1, bs, need_swap, &file_size);
    if (file_size)
        out_blocks(img, blocks[13], 2, bs, need_swap, &file_size);
    if (file_size)
        out_blocks(img, blocks[14], 3, bs, need_swap, &file_size);

    fclose(img);
    return EXIT_SUCCESS;
}