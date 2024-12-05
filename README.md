# CSE565_FinalProj

might come in handy:

https://adaptivesupport.amd.com/s/article/Vitis-Libraries-Compiling-and-Installing-OpenCV?language=en_US

### Timing (Benchmark 1)
- Base C++ code running on linux: 300 seconds
- Unoptimized/optimized vitis sw_EMU implementation: INSANELY LONG

### Timing (Benchmark 1 - EASIER)
- Base C++ code running on linux: 0.589 seconds
- Unoptimized vitis implementation:
  - sw_emu: 31.37 seconds
  - hw_emu: 
  - hw:
- Optimized vitis implementation:
  - sw_emu:  seconds
  - hw_emu:
  - hw:

Meaning of EASIER:
```
    const int LARGE_IMAGE_SIZE = 128;  // Reduced from 1000
    const int TEMPLATE_SIZE = 32;      // Reduced from 100
```

## ------- NOT COMPLETE ----------
### Timing (Benchmark 2)
- Base C++ code running on linux: 9769 seconds
- Unoptimized vitis implementation:
  - sw_emu: 300 seconds
  - hw_emu:
  - hw:
- Optimized vitis implementation: 
  - sw_emu: 300 seconds
  - hw_emu:
  - hw:
