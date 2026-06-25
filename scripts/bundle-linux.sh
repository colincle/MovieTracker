# This script builds and bundles a portable Linux AppImage for the Qt application.
# It is the Linux counterpart of scripts/bundle.sh (which builds a macOS .app).
#
# Required structure before running:
#
# - Root directory must contain:
#     - assets/                         → assets used by the app
#       └── appIcon/icon.png           → the app icon
#     - scripts/bundle-linux.sh        → this script (run it from the root directory)
#
# - Usage:
#     ./scripts/bundle-linux.sh <app_bundle_name>
#     (Example: ./scripts/bundle-linux.sh "ReWatch")
#
# - What it does:
#     - Builds the binary with cmake (-DSHIPPED=ON) in a temporary build-appimage/ dir
#     - Assembles an AppDir laid out so the SHIPPED asset path (../Resources/assets,
#       relative to the binary) resolves: usr/bin/ReWatch + usr/Resources/assets
#     - Generates a .desktop entry and installs the icon
#     - Runs linuxdeploy + its Qt plugin to embed Qt libraries and plugins
#     - Produces <app_bundle_name>-x86_64.AppImage in the root directory
#     - Cleans up build-appimage/ and AppDir/ on exit (the .AppImage is kept)
#
# - Requirements:
#     - qmake for Qt6 in PATH (qmake6 or qmake), or set $QMAKE
#     - curl (used to fetch linuxdeploy on first run; cached in ~/.cache/rewatch-bundle)
#     - FUSE, or the script falls back to APPIMAGE_EXTRACT_AND_RUN for headless hosts
#     - CMakeLists.txt must produce a binary named "ReWatch"


#!/bin/bash

set -e

if [ -z "$1" ]; then
	echo "Usage: $0 <bundle_name>"
	exit 1
fi

BUNDLE_NAME="$1"
BINARY_NAME="ReWatch"
ICON_NAME="rewatch"
ROOT_DIR="$(pwd)"
BUILD_DIR="$ROOT_DIR/build-appimage"
ICON_SRC="$ROOT_DIR/assets/appIcon/icon.png"
TOOLS_DIR="${REWATCH_TOOLS_DIR:-$HOME/.cache/rewatch-bundle}"
OUTPUT_NAME="${BUNDLE_NAME// /}.AppImage"

# Assemble the AppDir on a real local filesystem: linuxdeploy creates symlinks,
# which fail on VirtualBox shared folders (vboxsf). The finished .AppImage is
# moved back to the repo root at the end.
WORK_DIR="$(mktemp -d)"
APPDIR="$WORK_DIR/AppDir"

cleanup() {
	echo "Cleaning up..."
	rm -rf "$BUILD_DIR"
	rm -rf "$WORK_DIR"
}
trap cleanup EXIT

# Resolve qmake (Qt6)
QMAKE="${QMAKE:-$(command -v qmake6 || command -v qmake || true)}"
if [ -z "$QMAKE" ]; then
	echo "Error: qmake (Qt6) not found in PATH. Install Qt6 or set \$QMAKE." >&2
	exit 1
fi
export QMAKE

# Fetch linuxdeploy + Qt plugin (cached across runs)
mkdir -p "$TOOLS_DIR"
LINUXDEPLOY="$TOOLS_DIR/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_QT="$TOOLS_DIR/linuxdeploy-plugin-qt-x86_64.AppImage"

if [ ! -x "$LINUXDEPLOY" ]; then
	echo "Downloading linuxdeploy..."
	curl -fL -o "$LINUXDEPLOY" \
	    "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
	chmod +x "$LINUXDEPLOY"
fi
if [ ! -x "$LINUXDEPLOY_QT" ]; then
	echo "Downloading linuxdeploy-plugin-qt..."
	curl -fL -o "$LINUXDEPLOY_QT" \
	    "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"
	chmod +x "$LINUXDEPLOY_QT"
fi

# linuxdeploy finds its plugins on PATH; extract-and-run keeps it working without FUSE
export PATH="$TOOLS_DIR:$PATH"
export APPIMAGE_EXTRACT_AND_RUN=1

# Build binary (separate dir so it never clobbers the dev build's SHIPPED=OFF cache)
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake -DSHIPPED=ON -DCMAKE_BUILD_TYPE=Release ..
make -j"$(nproc)"
cd "$ROOT_DIR"

# Assemble the AppDir. Assets go under usr/Resources so that "../Resources/assets"
# (relative to usr/bin/ReWatch, the SHIPPED path) resolves at runtime.
rm -rf "$APPDIR"
mkdir -p "$APPDIR/usr/bin"
mkdir -p "$APPDIR/usr/Resources"
mkdir -p "$APPDIR/usr/share/applications"
mkdir -p "$APPDIR/usr/share/icons/hicolor/256x256/apps"

cp "$BUILD_DIR/$BINARY_NAME" "$APPDIR/usr/bin/"
chmod +x "$APPDIR/usr/bin/$BINARY_NAME"

cp -R assets "$APPDIR/usr/Resources/"

cp "$ICON_SRC" "$APPDIR/usr/share/icons/hicolor/256x256/apps/$ICON_NAME.png"
cp "$ICON_SRC" "$APPDIR/$ICON_NAME.png"

cat > "$APPDIR/usr/share/applications/$ICON_NAME.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=$BUNDLE_NAME
Exec=$BINARY_NAME
Icon=$ICON_NAME
Categories=Utility;
Terminal=false
EOF

# Deploy Qt and package the AppImage (run from the local work dir so the output,
# and every symlink linuxdeploy makes along the way, land on a symlink-capable fs)
export OUTPUT="$OUTPUT_NAME"
(
	cd "$WORK_DIR"
	"$LINUXDEPLOY" \
		--appdir "$APPDIR" \
		--executable "$APPDIR/usr/bin/$BINARY_NAME" \
		--desktop-file "$APPDIR/usr/share/applications/$ICON_NAME.desktop" \
		--icon-file "$APPDIR/$ICON_NAME.png" \
		--plugin qt \
		--output appimage
)

mv "$WORK_DIR/$OUTPUT_NAME" "$ROOT_DIR/$OUTPUT_NAME"
echo "Bundling complete: $ROOT_DIR/$OUTPUT_NAME"
