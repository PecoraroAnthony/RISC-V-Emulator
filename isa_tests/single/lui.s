.global _start

_start:
  # Test lui range 0x00 - 0xfffff
  lui x27, 0xf
  lui x28, 0xff
  lui x29, 0xfff
  lui x30, 0xffff
  lui x31, 0xfffff
