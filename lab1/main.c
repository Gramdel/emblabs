#include <stdio.h>
#include <inttypes.h>
#include <malloc.h>

struct __attribute__((packed)) section {
    char name[8];
    uint32_t virtual_size;
    uint32_t virtual_address;
    uint32_t raw_size;
    uint32_t raw_address;
    uint32_t reloc_address;
    uint32_t linenumbers;
    uint16_t relocations_number;
    uint16_t linenumbers_number;
    uint32_t characteristics;
};

int main() {
    printf("Input name of an executable file to be read (not more than 16 symbols):\n");
    char file_name[16];
    scanf("%s", file_name);
    FILE *file = fopen(file_name, "rb");
    if (!file) {
        printf("\nError: unable to open the file!\n");
        return -1;
    }

    uint16_t number_of_sections;
    uint32_t addr_of_entry_point, base_of_code;
    uint64_t image_base;

    fseek(file, 134, SEEK_SET);
    fread(&number_of_sections, sizeof(uint16_t), 1, file);
    fseek(file, 168, SEEK_SET);
    fread(&addr_of_entry_point, sizeof(uint32_t), 1, file);
    fread(&base_of_code, sizeof(uint32_t), 1, file);
    fread(&image_base, sizeof(uint64_t), 1, file);

    struct section *sections = malloc(sizeof(struct section) * number_of_sections);
    if (!sections) {
        printf("\nError: can't allocate memory for sections!\n");
        return -2;
    }

    fseek(file, 0x188, SEEK_SET);
    for (uint16_t i = 0; i < number_of_sections; i++) {
        fread(&sections[i], sizeof(struct section), 1, file);
    }

    fseek(file, (long) sections[0].raw_address, SEEK_SET);
    uint8_t* data = malloc(sections[0].raw_size);
    if (!data) {
        printf("\nError: can't allocate memory for binary data!\n");
        return -2;
    }
    fread(data, sections[0].raw_size, 1, file);
    fclose(file);
    file = fopen("code.bin", "wb");
    fwrite(data, sections[0].raw_size, 1, file);

    fclose(file);
    file = fopen("info.txt", "w");
    fprintf(file, "[INFORMATION ABOUT FILE \"%s\"]\n", file_name);
    fprintf(file, "Address of Entry Point: 0x%X\n", addr_of_entry_point);
    fprintf(file, "Number of sections:\t%" PRIu16 "\n", number_of_sections);
    for (uint16_t i = 0; i < number_of_sections; i++) {
        fprintf(file, "\n\t[SECTION %" PRIu16 "]\n", i + 1);
        fprintf(file, "Name:\t\t\t%s\n", sections[i].name);
        fprintf(file, "Virtual Size:\t\t0x%X\n", sections[i].virtual_size);
        fprintf(file, "Virtual Address:\t0x%X\n", sections[i].virtual_address);
        fprintf(file, "Raw Size:\t\t0x%X\n", sections[i].raw_size);
        fprintf(file, "Raw Address:\t\t0x%X\n", sections[i].raw_address);
        fprintf(file, "Reloc Address:\t\t0x%X\n", sections[i].reloc_address);
        fprintf(file, "Linenumbers:\t\t0x%X\n", sections[i].linenumbers);
        fprintf(file, "Relocations Number:\t0x%X\n", sections[i].relocations_number);
        fprintf(file, "Linenumbers Number:\t0x%X\n", sections[i].linenumbers_number);
        fprintf(file, "Characteristics:\t0x%X\n", sections[i].characteristics);
    }

    fclose(file);


    free(data);
    free(sections);
    return 0;
}
