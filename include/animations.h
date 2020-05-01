#include <Arduino.h>

// animation definitions
const uint64_t ANIM_lines[] PROGMEM = {
  0x1818181818181818,
  0x0c0c181818183030,
  0x060c0c1818303060,
  0x03060c18183060c0,
  0x0001071e78e08000,
  0x0000033ffcc00000,
  0x000000ffff000000,
  0x0000c0fc3f030000,
  0x0080e0781e070100,
  0x80c060381c060301,
  0x60303018180c0c06,
  0x3030181818180c0c
};
int ANIM_lines_len = sizeof(ANIM_lines)/8;

const uint64_t ANIM_bomb[] PROGMEM = {
  0x0000000000000018,
  0x000000000000183c,
  0x0000000000183c3c,
  0x00000000183c3c3c,
  0x000000183c3c3c3c,
  0x0000183c3c3c3c00,
  0x00183c3c3c3c0000,
  0x183c3c3c3c000000,
  0x3c3c3c3c00000000,
  0x3c7e341c00000000,
  0x3c5ae60001000000,
  0x3c5af62881000000,
  0x1818b166118a4000,
  0x0000099421025024,
  0x000000000050810a,
  0x0000000000000000
};
int ANIM_bomb_len = sizeof(ANIM_bomb)/8;

const uint64_t ANIM_beacon[] PROGMEM = {
  0x7e7e7e7e3c180000,
  0x7c7c7c7c385a8124,
  0x7a7a7a7a345a8124,
  0x7676767634992442,
  0x6e6e6e6e2c992442,
  0x5e5e5e5e2c180000,
  0x3e3e3e3e1c180000
};
int ANIM_beacon_len = sizeof(ANIM_beacon)/8;

const uint64_t ANIM_gun[] PROGMEM = {
  0x00060e167e7d0000,
  0x00060e167e7e0000,
  0x000103051f1f0000,
  0x0000000107070000,
  0x0000000001010000,
  0x0000000203030000,
  0x0000040205030400,
  0x0008040009010408,
  0x1008000011010000,
  0x0000000021010000,
  0x0000000041010000,
  0x0000000081010000,
  0x0000000001010000
};
int ANIM_gun_len = sizeof(ANIM_gun)/8;

const uint64_t ANIM_laser[] PROGMEM = {
  0x0102028241404080,
  0x01030283c140c080,
  0x01030383c1c0c080,
  0x0102078241e04080,
  0x01020f8241f04080,
  0x01021e8241784080,
  0x01023a82415c4080,
  0x01027282414e4080,
  0x0102e28241474080,
  0x0102c28241434080,
  0x0102828241414080,
  0x0102028241404080
};
int ANIM_laser_len = sizeof(ANIM_laser)/8;

const uint64_t ANIM_pulsating[] PROGMEM = {
  0x8100000000000081,
  0x8142000000004281,
  0x0042240000244200,
  0x0000241818240000,
  0x0000001818000000,
  0x0000183c3c180000,
  0x0018244242241800,
  0x1842008181004218
};
int ANIM_pulsating_len = sizeof(ANIM_pulsating)/8;

const uint64_t ANIM_fx1[] PROGMEM = {
  0x0101010101010101,
  0x0103030303030301,
  0x0103070707070301,
  0x0103070f0f070301,
  0x0002061e1e060200,
  0x0000243c3c240000,
  0x0040607878604000,
  0x80c0e0f0f0e0c080,
  0x80c0e0e0e0e0c080,
  0x80c0c0c0c0c0c080,
  0x8080808080808080,
  0x4040808080808080,
  0x0020204040408080,
  0x0010102020404080,
  0x0000081020204080,
  0x0000040810204080,
  0x0000000408304080,
  0x0000000006186080,
  0x00000000000638c0,
  0x00000000000003fc,
  0x00000000000000ff,
  0x00000000000081ff,
  0x000000000081c3ff,
  0x0000000081c3e7ff,
  0x00000081c3e7ff7e,
  0x000081c3e7ff7e3c,
  0x0081c3e7ff7e3c18,
  0x81c3e7ff7e3c1800,
  0xc3e7ff7e3c180000,
  0xe7ff7e3c18000000,
  0xff7e3c1800000000,
  0x7e3c180000000000,
  0x3c18000000000000,
  0x1800000000000000,
  0x0000000000000000
};
int ANIM_fx1_len = sizeof(ANIM_fx1)/8;

const uint64_t ANIM_fx2[] PROGMEM = {
  0x8142241818244281,
  0x4021121c38488402,
  0x2010111e78880804,
  0x1010101ff8080808,
  0x080808f81f101010,
  0x040888781e111020,
  0x028448381c122140
};
int ANIM_fx2_len = sizeof(ANIM_fx2)/8;


const uint64_t ANIM_FACE_lol[] PROGMEM = {
  0x00003c4200666600,
  0x003c7e4200660000,
  0x3c427e4200006600,
  0x003c427e42006600,
  0x3c427e4200660000,
  0x003c427e42006600,
  0x3c427e4200660000,
  0x003c427e42006600,
  0x3c427e4200006600,
  0x3c427e4200666600,
  0x003c7e4200666600
};
int ANIM_FACE_lol_length = sizeof(ANIM_FACE_lol)/8;

const uint64_t ANIM_FACE_sad[] PROGMEM = {
  0x00003c0000666600,
  0x0018241800666600,
  0x0018241800666600,
  0x0018241800666600,
  0x0018241800666600,
  0x0018241800660000,
  0x00423c0066000000,
  0x00423c0066000000,
  0x00423c0066000000,
  0x00423c0066000000,
  0x0000423c00660000,
  0x00423c0066000000,
  0x00423c0066000000,
  0x00423c0066000000,
  0x00423c0066000000
};
int ANIM_FACE_sad_length = sizeof(ANIM_FACE_sad)/8;