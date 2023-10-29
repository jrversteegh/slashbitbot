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
  printk("Setup flash storage with %d pages of %d bytes.\n", fs.sector_count, fs.sector_size);
}

void store_object(int id, void const* object, size_t size) {
  printk("Writing object: %d with size %d\n", id, size);
  int ec = nvs_write(&fs, id, object, size);
  if (ec < 0) {
    error(3, "Failed to write object %d to flash, error: %d.", id, ec);
    return;
  }
  printk("Written object id: %d with size %d to flash\n", id, size);
}

bool load_object(int id, void* object, size_t size) {
  int ec = nvs_read(&fs, id, object, size);
  if (ec <= 0) {
    printk("Failed to read object %d from flash, error %d.\n", id, ec);
    return false;
  }
  return true;
}
