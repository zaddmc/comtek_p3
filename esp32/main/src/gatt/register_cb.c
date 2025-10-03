#include "gatt/register_cb.h"
#include "commmon.h"

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {
  /* Local variables */
  char buf[BLE_UUID_STR_LEN];

  /* Handle GATT attributes register events */
  switch (ctxt->op) {
  /* Service register event */
  case BLE_GATT_REGISTER_OP_SVC:
    ESP_LOGD(TAG, "SERVICE REGISTER %s with handle=%d",
             ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf), ctxt->svc.handle);
    break;

  /* Characteristic register event */
  case BLE_GATT_REGISTER_OP_CHR:
    ESP_LOGD(TAG,
             "CHARACTERISTIC REGISTER %s with "
             "DESCRIPTOR HANDLE=%d VAL HANDLE=%d",
             ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
             ctxt->chr.def_handle, ctxt->chr.val_handle);
    break;

  /* Descriptor register event */
  case BLE_GATT_REGISTER_OP_DSC:
    ESP_LOGD(TAG, "DESCRIPTOR HANDLE%s with handle=%d",
             ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf), ctxt->dsc.handle);
    break;

  /* Unknown event */
  default:
    assert(0);
    break;
  }
}
