DESCRIPTION = "SDK tools for Qt/[X11|Mac|Embedded] version 4.x"
DEPENDS = "zlib-nativesdk dbus-nativesdk qt4-native"
# note needs a space at the end for some reason...
DEPENDS += "${@base_contains('DISTRO_FEATURES', 'x11', 'libx11-nativesdk', '', d)} "
SECTION = "libs"
HOMEPAGE = "http://qt.nokia.com"
LICENSE = "LGPLv2.1 | GPLv3"
INC_PR = "r13"

FILESEXTRAPATHS =. "${FILE_DIRNAME}/qt4-${PV}:"

inherit nativesdk qmake2

SRC_URI = "http://releases.qt-project.org/qt4/source/qt-everywhere-opensource-src-${PV}.tar.gz \
           file://0001-qlibraryinfo-allow-to-set-qt.conf-from-the-outside-u.patch \
           file://0004-configure-qmake-is-already-built-in-qt4-tools-native.patch \
           file://0005-configure-set-LFLAGS-to-pick-up-zlib-from-staging.patch \
           file://0006-configure-Use-OE_QMAKE_-values-to-specify-Qt-utility.patch \
           file://0007-dbus-Remove-const-usage-that-causes-compile-failure-.patch \
           file://0008-qmake.pro-Allow-building-a-separate-qmake-for-the-ta.patch \
           file://g++.conf \
           file://linux.conf"

S = "${WORKDIR}/qt-everywhere-opensource-src-${PV}"

LIC_FILES_CHKSUM = "file://LICENSE.LGPL;md5=fbc093901857fcd118f065f900982c24 \
                    file://LICENSE.GPL3;md5=babc5b6b77441da277f5c06b2e547720 \
                    file://LGPL_EXCEPTION.txt;md5=411080a56ff917a5a1aa08c98acae354"

require qt4_arch.inc

# FIXME: make it work with "${STAGING_BINDIR_NATIVE}/pkg-config --cflags dbus-1"
EXTRA_OECONF = "-prefix ${prefix} \
                -qt-libjpeg -system-zlib \
                -no-libjpeg -no-libpng -no-libmng -no-libtiff \
                -no-accessibility \
                -no-cups \
                -no-exceptions  \
                -no-nas-sound \
                -no-nis -no-openssl \
                -verbose -release -fast -static \
                -platform ${TARGET_OS}-oe-g++ \
                -xplatform ${TARGET_OS}-oe-g++ \
                -arch ${QT_ARCH} \
                -embedded -no-freetype -no-glib -no-iconv \
                -qt3support \
                -I${STAGING_DIR_HOST}${SDKPATHNATIVE}/usr/include/dbus-1.0 \
                -I${STAGING_DIR_HOST}${SDKPATHNATIVE}/usr/lib/dbus-1.0/include"

# yank default -e, otherwise we get the following error:
# moc_qbuffer.cpp: No such file or directory
EXTRA_OEMAKE = " "

do_configure() {
    # Avoid problems with Qt 4.8.0 configure setting QMAKE_LINK from LD (since we want the linker to be g++)
    unset LD

    if [ ! -e mkspecs/${TARGET_OS}-oe-g++ ]; then
        ln -sf linux-g++ mkspecs/${TARGET_OS}-oe-g++
    fi

    cp ../g++.conf mkspecs/common
    cp ../linux.conf mkspecs/common

    if [ -f mkspecs/common/g++-base.conf ] ; then
        # don't use host g++ even during configure (4.8.0+)
        sed -i -e "s# g++# ${CXX}#" mkspecs/common/g++-base.conf
        sed -i -e "s# gcc# ${CC}#" mkspecs/common/g++-base.conf
    fi

    # Use qmake from qt4-native for building
    if [ ! -e bin/qmake ]; then
        ln -sf ${STAGING_BINDIR_NATIVE}/qmake2 bin/qmake
    fi
    set_arch
    (echo o; echo yes) | CC="${CC}" CXX="${CXX}" ./configure ${EXTRA_OECONF} || die "Configuring qt failed. EXTRA_OECONF was ${EXTRA_OECONF}"
}

TOBUILD = "\
  src/tools/bootstrap \
  src/tools/moc \
  src/corelib \
  src/sql \
  src/dbus \
  src/qt3support \
  src/xml \
  src/tools/uic \
  src/tools/rcc \
  src/network \
  src/gui \
  src/tools/uic3 \
  tools/linguist/lrelease \
  tools/linguist/lupdate \
  tools/qdbus \
"

do_compile() {
    for i in ${TOBUILD}; do
        cd ${S}/$i && oe_runmake CC="${CC}" CXX="${CXX}"
    done

    # Build nativesdk qmake
    export QMAKESPEC="${S}/mkspecs/${TARGET_OS}-oe-g++"
    cd ${S}/qmake
    ${OE_QMAKE_QMAKE}
    oe_runmake CC="${CC}" CXX="${CXX}"
    cd ${S}
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 bin/qmake2 ${D}${bindir}/qmake2
    for i in moc uic uic3 rcc lrelease lupdate qdbuscpp2xml qdbusxml2cpp; do
        install -m 0755 bin/${i} ${D}${bindir}/${i}4
    done

    (cd ${D}${bindir}; \
    ln -s qmake2 qmake; \
    for i in moc uic uic3 rcc lrelease lupdate qdbuscpp2xml qdbusxml2cpp; do \
        ln -s ${i}4 ${i}; \
    done)
}

RDEPENDS_${PN} += "\
	nativesdk-bison \
"
