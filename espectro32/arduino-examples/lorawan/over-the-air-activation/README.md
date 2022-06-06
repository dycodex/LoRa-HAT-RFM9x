# LoRaWAN Over-the-Air-Activation Example

## Requirements

Pertama-tama, install library berikut dengan cara menyalin folder library yang disebutkan ke folder library Arduino. Jika Anda menggunakan Windows, maka folder library berada di `C:\Users\USERNAME\Documents\Arduino\libraries`.

Library yang harus di-install:
* lmic-catena, salin folder library ini dari folder `platformio-examples/lorawan/over-the-air-activation/lib` ke `C:\Users\USERNAME\Documents\Arduino\libraries`

## Configuration

Ubah nilai `DEVICE_LRWAN_DEVEUI` dan `DEVICE_LRWAN_APPKEY` yang didapat dari LoRaWAN network server, lihat di file `lorawan_config.h`.

