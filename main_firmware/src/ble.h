#ifndef BLE_H_
#define BLE_H_

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <String.h>

String device_name = "SCALLER-"+String((uint16_t)ESP.getEfuseMac(), HEX);

bool deviceConnected = true;

BLEServer* pServer = NULL;
BLECharacteristic* WeightCharacteristic = NULL;
BLECharacteristic* BattMonCharacteristic = NULL;

// #define HR_SERVICE_UUID        "0000181d-0000-1000-8000-00805f9b34fb"
// #define HR_CHARACTERISTIC_UUID "00002a98-0000-1000-8000-00805f9b34fb"

static BLEUUID WEIGHT_SERVICE_UUID(BLEUUID((uint16_t)0x181D));
static BLEUUID WEIGHT_CHARACTERISTIC_UUID(BLEUUID((uint16_t)0x2A98)); //2A9D //0x032 //0x0C80 to 0x0CBF

// #define BATT_UUID              "0000180f-0000-1000-8000-00805f9b34fb"
// #define BATT_C_UUID            "00002a19-0000-1000-8000-00805f9b34fb"

static BLEUUID BATT_UUID(BLEUUID((uint16_t)0x180F));
static BLEUUID BATT_C_UUID(BLEUUID((uint16_t)0x2A19));

#endif