inline uint32_t col_diff(int32_t r1, int32_t g1, int32_t b1, int32_t r2, int32_t g2, int32_t b2) {
  r1 -= r2;
  r1 *= r1;
  g1 -= g2;
  g1 *= g1;
  b1 -= b2;
  b1 *= b1;
  return r1 + g1 + b1;
}

inline uint8_t idx_nearest_colour(uint8_t r, uint8_t g, uint8_t b) {
  uint8_t ret = 0;
  uint32_t best_mag = 0xFFFFFFFFUL;
  for (uint16_t i = 0; i < 256; i++) {
    uint32_t mag = col_diff(r, g, b, im_cm[i][2], im_cm[i][1], im_cm[i][0]);
    if (mag < best_mag) {
      ret = i;
      best_mag = mag;
    }
  }
  return ret;
}

bool decode_jpg() {
  if (!jpg_img) imready = false;
  if (!jpg_sz) imready = false;
  if (!imready) return imready;

  if (jpeg.openRAM(jpg_img, jpg_sz, drawMCUs)) {
    DBGPORT.println("Valid JPG");
    DBGPORT.printf("Image size: %d x %d, orientation: %d, bpp: %d\r\n", jpeg.getWidth(), jpeg.getHeight(), jpeg.getOrientation(), jpeg.getBpp());
    jpeg.setPixelType(RGB565_LITTLE_ENDIAN);
    if (jpeg.decode(0, 0, 0)) {
      DBGPORT.println("JPG decoded");
      imready = true;
    } else {
      DBGPORT.println("JPG decode error");
      imready = false;
    }
    jpeg.close();
  } else {
    DBGPORT.println("Invalid JPG");
    imready = false;
  }
  return imready;
}

bool take_photo() {
  imready = false;
  if (!jpg_img) return imready;

  if (!cam.takePicture()) {
    DBGPORT.println("Failed to snap!");
    return imready;
  }

  // Get the size of the image (frame) taken
  jpg_sz = cam.frameLength();
  DBGPORT.print(jpg_sz, DEC);
  DBGPORT.println(" byte image");

  // Since we use pre-allocated memory, the maximum size is fixed
  if (jpg_sz > MAX_ALLOWED_JPG_SIZE) {
    DBGPORT.println("Img size too big!");
    return imready;
  }

  uint16_t remaining = jpg_sz;
  uint8_t *jpg_img_p = jpg_img;
  while (remaining) {
    uint8_t *buf;
    uint8_t bytesToRead = min((uint16_t)64, remaining);
    buf = cam.readPicture(bytesToRead);
    memcpy(jpg_img_p, buf, bytesToRead);
    jpg_img_p += bytesToRead;
    remaining -= bytesToRead;
  }

  DBGPORT.println("Img read!");
  imready = true;

  cam.resumeVideo();

  return imready;
}

void task_camera() {
  unsigned long curr_time = millis();
  static unsigned long last_time = 0;

  // This condition mirrors that of the SSTV module but modified to run 1 minute earlier
  if (sstv_run_now || (!inhibit_sstv && ((((tmin + 1) % SSTV_mod_m) == 0) && (curr_time - last_time) >= ((SSTV_mod_m * 60000L) - 60000L)))) {
    take_photo();
    last_time = curr_time;
  }
}

void setup_camera() {
  Serial1.begin(115200, SERIAL_8N1, PIN_CAM_RX, PIN_CAM_TX);
  cam.reset();
  delay(500);
  Serial1.begin(38400, SERIAL_8N1, PIN_CAM_RX, PIN_CAM_TX);
  if (cam.reset()) {
    Serial.println("Camera Found:");
  } else {
    Serial.println("No camera found?");
    return;
  }

  // Print out the camera version information (optional)
  char *reply = cam.getVersion();
  if (reply == 0) {
    Serial.print("Failed to get version");
  } else {
    Serial.println("-----------------");
    Serial.print(reply);
    Serial.println("-----------------");
  }

  cam.setImageSize(VC0706_320x240);
  // You can read the size back from the camera (optional, but maybe useful?)
  uint8_t imgsize = cam.getImageSize();
  Serial.print("Image size: ");
  if (imgsize == VC0706_640x480) Serial.println("640x480");
  if (imgsize == VC0706_320x240) Serial.println("320x240");
  if (imgsize == VC0706_160x120) Serial.println("160x120");

  // Switch to 115200
  cam.setBaud115200();
  delay(500);
  Serial1.begin(115200, SERIAL_8N1, PIN_CAM_RX, PIN_CAM_TX);
}
