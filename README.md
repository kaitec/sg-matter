# SolarGaps Matter firmware

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Repository folder path

~/esp-matter/examples/sg-matter

## 2. Working process

cd ~/esp/esp-idf; ./install.sh; source ./export.sh
cd ~/matter/esp-matter; ./install.sh; . ./export.sh

export IDF_CCACHE_ENABLE=1

cd ~/matter/esp-matter/examples/sg-matter

idf.py build
idf.py flash monitor
