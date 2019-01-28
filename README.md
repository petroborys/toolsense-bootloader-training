Download mbed libraries

```
mbed cache on
mbed update
```

Compile the bootloader

```
mbed compile -t GCC_ARM -m UBLOX_C030_N211 --source=./bootloader -c
```

Compile the application

```
mbed compile -t GCC_ARM -m UBLOX_C030_N211
```

Objectives:
1. Use your GitHub profile for development
2. Upgrade `mbed-os` from 5.8.4 to 5.11.2
3. Fix `bootloader/.mbedignore`
4. Upgrade `flashiap-driver` and `sd-driver`
5. Add as much stuff as possible to `bootloader/.mbedignore`, to make the build faster
