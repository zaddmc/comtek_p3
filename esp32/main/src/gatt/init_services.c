#include "gatt/init_services.h"
#include "gatt/write_svc_test.h"
#include "services/gatt/ble_svc_gatt.h"
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    /* Heart rate service */
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = &test_svc_uuid.u,
     .characteristics =
         (struct ble_gatt_chr_def[]){
             {/* Heart rate characteristic */
              .uuid = &write_chr_uuid.u,
              .access_cb = write_chr_access,
              .flags = BLE_GATT_CHR_F_WRITE,
              .val_handle = &write_chr_handle},
             {
                 0, /* No more characteristics in this service. */
             }}},

    {
        0, /* No more services. */
    },
};
int gatt_svc_init(void) {

  /* Local variables */
  int status_code;

  /* 1. GATT service initialization */
  ble_svc_gatt_init();

  /* 2. Update GATT services counter */
  status_code = ble_gatts_count_cfg(gatt_svr_svcs);
  if (status_code != 0) {
    return status_code;
  }

  /* 3. Add GATT services */
  status_code = ble_gatts_add_svcs(gatt_svr_svcs);
  if (status_code != 0) {
    return status_code;
  }

  return 0;
}
