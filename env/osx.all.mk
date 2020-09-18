CC := clang++
CFLAGS := $(CFLAGS:-s=)

LIB_DIRS := \
	/usr/local/lib \
	/Users/dane/repositories/box2d/build/src

INCLUDE_DIRS := \
	/usr/local/include \
	/Users/dane/repositories/imgui \
	/Users/dane/repositories/imgui-sfml/build/include \
	/Users/dane/repositories/box2d/include

BUILD_FLAGS :=

MACOS_FRAMEWORK_PATHS := \
	/Library/Frameworks \
	/System/Library/Frameworks

# Name, no extension (eg. CoreFoundation, ogg)
MACOS_FRAMEWORKS := \
	CoreFoundation \
	OpenGL

# Icon .png
PRODUCTION_MACOS_ICON := sfml

PRODUCTION_DEPENDENCIES := \
	$(PRODUCTION_DEPENDENCIES)

PRODUCTION_MACOS_BUNDLE_DEVELOPER := danebulat
PRODUCTION_MACOS_BUNDLE_DISPLAY_NAME := SFML Box2D Boilerplate
PRODUCTION_MACOS_BUNDLE_NAME := SFML Box2D Boilerplate
PRODUCTION_MACOS_MAKE_DMG := true
PRODUCTION_MACOS_BACKGROUND := dmg-background

PRODUCTION_MACOS_DYLIBS := \
	/usr/local/lib/libsfml-graphics.2.5 \
	/usr/local/lib/libsfml-audio.2.5 \
	/usr/local/lib/libsfml-network.2.5 \
	/usr/local/lib/libsfml-window.2.5 \
	/usr/local/lib/libsfml-system.2.5 \
	/Users/dane/repositories/box2d/build/src/libbox2d

# Path, no extension (eg. /Library/Frameworks/ogg)
PRODUCTION_MACOS_FRAMEWORKS := \
	/Library/Frameworks/freetype \
	/Library/Frameworks/FLAC \
	/Library/Frameworks/OpenAL \
	/Library/Frameworks/freetype \
	/Library/Frameworks/ogg \
	/Library/Frameworks/vorbis \
	/Library/Frameworks/vorbisenc \
	/Library/Frameworks/vorbisfile
