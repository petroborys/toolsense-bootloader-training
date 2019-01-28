// Makes sure that this file is not included when building the application
#if !defined(BOOTLOADER_ADDR) && defined(POST_APPLICATION_ADDR)

#include "FlashIAPBlockDevice.h"
#include "LittleFileSystem.h"
#include "SDBlockDevice.h"
#include "FATFileSystem.h"
#include "mbed.h"

#define FS_MOUNT_PATH "fs"
#define FS_UPDATE_FILE_PATH "/" FS_MOUNT_PATH "/" TS_UPDATE_FILE_NAME

#define SD_MOUNT_PATH "sd"
#define SD_UPDATE_FILE_PATH "/" SD_MOUNT_PATH "/" TS_UPDATE_FILE_NAME

FlashIAPBlockDevice flashiap_bd(TS_FLASH_BD_START, TS_FLASH_BD_SIZE);
LittleFileSystem flashiap_fs(FS_MOUNT_PATH, &flashiap_bd);

SDBlockDevice sd_bd(SDMOSI, SDMISO, SDSCK, SDCS);
FATFileSystem sd_fs(SD_MOUNT_PATH, &sd_bd);

FlashIAP mcuflash;
DigitalOut led(LED_RED);

bool update(const char *path);
void apply_update(FILE *file, uint32_t address);

int main() {
    bool res = update(FS_UPDATE_FILE_PATH);

    if (!res) {
        update(SD_UPDATE_FILE_PATH);
    }

    mbed_start_application(POST_APPLICATION_ADDR);
}

bool update(const char *path) {
    bool res = false;

    FILE *file = fopen(path, "rb");
    if (file != NULL) {
        mcuflash.init();
        apply_update(file, POST_APPLICATION_ADDR);
        fclose(file);
        remove(path);
        mcuflash.deinit();
        res = true;
    }

    return res;
}

void apply_update(FILE *file, uint32_t address) {
    const uint32_t page_size = mcuflash.get_page_size();
    char *page_buffer = new char[page_size];

    uint32_t addr = address;
    uint32_t next_sector = addr + mcuflash.get_sector_size(addr);
    bool sector_erased = false;
    size_t pages_flashed = 0;

    while (true) {

        // Read data for this page
        memset(page_buffer, 0, sizeof(page_buffer));
        int size_read = fread(page_buffer, 1, page_size, file);
        if (size_read <= 0) {
            break;
        }

        // Erase this page if it hasn't been erased
        if (!sector_erased) {
            mcuflash.erase(addr, mcuflash.get_sector_size(addr));
            sector_erased = true;
        }

        // Program page
        mcuflash.program(page_buffer, addr, page_size);

        addr += page_size;
        if (addr >= next_sector) {
            next_sector = addr + mcuflash.get_sector_size(addr);
            sector_erased = false;
        }

        if (++pages_flashed % 24 == 0) {
            led = !led;
        }
    }
    led = 0;

    delete[] page_buffer;
}

#endif
