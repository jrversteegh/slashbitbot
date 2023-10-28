#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/fs/nvs.h>

#include "storage.h"
#include "errors.h"


static nvs_fs fs;

void initialize_storage() {
  flash_pages_info info;
  fs.flash_device = FIXED_PARTITION_DEVICE(storage_partition);
  fs.offset = FIXED_PARTITION_OFFSET(storage_partition);
  if (!device_is_ready(fs.flash_device)) {
    error(3, "Flash device %s is not ready.", fs.flash_device->name);
    return;
  }
  if (flash_get_page_info_by_offs(fs.flash_device, fs.offset, &info) != 0) {
    error(3, "Failed to get flash page info for  %s.", fs.flash_device->name);
    return;
  }
  // Should add up to 8kiB
  fs.sector_size = info.size;
  fs.sector_count = 2;
  if (nvs_mount(&fs) != 0) {
    error(3, "Failed to mount flash file system.");
    return;
  }
}

void store_object(int id, void* object, size_t size) {
  if (auto ec = nvs_write(&fs, id, object, size) < 0) {
    error(3, "Failed to write object to flash: %d.", ec);
    return;
  }
  printk("Written object id: %d with size %d to flash", id, size);
}

bool load_object(int id, void* object, size_t size) {
  if (auto ec = nvs_read(&fs, id, object, size) <= 0) {
    printk("Failed to read object from flash: %d.", ec);
    return false;
  }
  return true;
}
