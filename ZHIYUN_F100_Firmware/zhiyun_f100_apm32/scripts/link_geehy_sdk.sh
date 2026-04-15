#!/bin/sh
# Symlink Geehy SDK sources into src/geehy_sdk/ (relative links, safe for git).
# Run from project root: ./scripts/link_geehy_sdk.sh
set -e
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DST="$ROOT/src/geehy_sdk"
REL_SDK="../../../../SDK/APM32F10x_SDK_V2.0.0"
STD="$ROOT/../../SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/src"
DEV="$ROOT/../../SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Source"

if [ ! -d "$ROOT/../../SDK/APM32F10x_SDK_V2.0.0" ]; then
  echo "SDK not found next to ZHIYUN_F100_Firmware (expected ZHIYUN_F100/SDK/...)"
  exit 1
fi

mkdir -p "$DST"

# Keep documentation files; only clear generated source symlinks.
rm -f "$DST"/*.c "$DST"/*.S

cd "$DST"
for f in "$STD"/*.c; do
  base="$(basename "$f")"
  ln -sf "$REL_SDK/Libraries/APM32F10x_StdPeriphDriver/src/$base" "$base"
done
ln -sf "$REL_SDK/Libraries/Device/Geehy/APM32F10x/Source/system_apm32f10x.c" system_apm32f10x.c
ln -sf "$REL_SDK/Libraries/Device/Geehy/APM32F10x/Source/gcc/startup_apm32f103xb.S" startup_apm32f103xb.S

echo "Linked $(ls -1 | wc -l | tr -d ' ') files into src/geehy_sdk/"
