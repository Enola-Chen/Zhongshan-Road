#ifndef _FIREBASE_UPLOAD_H
#define _FIREBASE_UPLOAD_H

// Uploads people count to Firebase /devices/<STATION_ID>.
// Build env: grandmaShop | porkRice (see shared/stations/).

void firebase_init(void);
void firebase_upload(int wifi_count, int ble_count, int pax_count);

#endif // _FIREBASE_UPLOAD_H
