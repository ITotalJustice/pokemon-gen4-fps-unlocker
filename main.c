#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define KEY_FR_VALUE    0x6325  // 0x2563 byteswapped.
#define MAGIC_TOTAL     5
#define MAGIC_LEN       12

const char ROM_MAGIC[MAGIC_TOTAL][MAGIC_LEN] =
{
    "POKEMON P",
    "POKEMON D",
    "POKEMON PL",
    "POKEMON HG",
    "POKEMON SS",
};

int32_t ROM_OFFSETS[] =
{
    0x4DB0, // DIAMOND
    0x4DB0, // PEAL
    0x4DF8, // PLAT
    0x4E28, // HG
    0x4E28, // SS
};


int32_t get_offset(FILE* fp)
{
    if (!fp) return -1;

    char magic[MAGIC_LEN] = { 0 };

    fseek(fp, 0, SEEK_SET);
    if (!fread(magic, MAGIC_LEN, 1, fp))
    {
        printf("FAILED TO READ FILE\n\n");
        return -1;
    }

    for (uint8_t i = 0; i < MAGIC_TOTAL; i++)
    {
        if (strcmp(magic, ROM_MAGIC[i]) == 0)
        {
            printf("FOUND GAME: %s\n\n", ROM_MAGIC[i]);
            printf("OFFSET: 0x%X\n\n", ROM_OFFSETS[i]);
            return ROM_OFFSETS[i];
        }
    }

    printf("FAILED TO FIND MAGIC\n\n");
    return -1;
}

int change_frame_rate(FILE* fp, int32_t offset)
{
    if (!fp || !offset) return -1;

    uint16_t cur_value = 0;
    fseek(fp, offset, SEEK_SET);
    if (!fread(&cur_value, 2, 1, fp))
    {
        printf("FAILED TO READ FILE\n\n");
        return -1;
    }

    if (cur_value != 0 && cur_value != KEY_FR_VALUE)
    {
        printf("UNKOWN CURRENT VALUE: 0x%X\n\n", cur_value);
        return -1;
    }

    printf("%s\n\n", cur_value ? "REMOVING FRAMERATE CAP" : "ENABLEING FRAMERATE CAP");

    uint16_t value = cur_value ? 0000 : KEY_FR_VALUE;
    fseek(fp, offset, SEEK_SET);
    if (!fwrite(&value, 2, 1, fp))
    {
        printf("FAILED TO WRITE FILE\n\n");
        return -1;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    printf("\nWELCOME!\n\n");

    if (argc != 2)
    {
        printf("\nUSAGE: romname\n\n");
        printf("EXAMPLE: pokemon_diamond.rom\n\n");
        return -1;
    }

    FILE* fp = fopen(argv[1], "rb+");
    if (!fp)
    {
        printf("FAILED TO OPEN FILE %s\n\n", argv[1]);
        return -1;
    }

    int32_t offset = get_offset(fp);
    if (offset == -1)
    {
        fclose(fp);
        return -1;
    }

    if (change_frame_rate(fp, offset) == -1)
    {
        fclose(fp);
        return -1;
    }

    printf("DONE!\n\n");
    fclose(fp);
    return 0;
}