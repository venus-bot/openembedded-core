SUMMARY = "Tools for managing memory technology devices"
SECTION = "base"
DEPENDS = "zlib lzo e2fsprogs util-linux"
HOMEPAGE = "http://www.linux-mtd.infradead.org/"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://COPYING;md5=0636e73ff0215e8d672dc4c32c317bb3 \
                    file://include/common.h;beginline=1;endline=17;md5=ba05b07912a44ea2bf81ce409380049c"

# Use the latest version at 26 Oct, 2013
SRCREV = "dcea43eba91642939c82739387147da26d572758"
SRC_URI = "git://git.infradead.org/mtd-utils.git \
		file://add-exclusion-to-mkfs-jffs2-git-2.patch \
"

PV = "1.5.0+git${SRCPV}"

S = "${WORKDIR}/git/"

EXTRA_OEMAKE = "'CC=${CC}' 'RANLIB=${RANLIB}' 'AR=${AR}' 'CFLAGS=${CFLAGS} -I${S}/include -DWITHOUT_XATTR' 'BUILDDIR=${S}'"

do_install () {
	oe_runmake install DESTDIR=${D} SBINDIR=${sbindir} MANDIR=${mandir} INCLUDEDIR=${includedir}
}

PACKAGES =+ "mtd-utils-jffs2 mtd-utils-ubifs mtd-utils-misc"

FILES_mtd-utils-jffs2 = "${sbindir}/mkfs.jffs2 ${sbindir}/jffs2dump ${sbindir}/jffs2reader ${sbindir}/sumtool"
FILES_mtd-utils-ubifs = "${sbindir}/mkfs.ubifs ${sbindir}/ubi*"
FILES_mtd-utils-misc = "${sbindir}/nftl* ${sbindir}/ftl* ${sbindir}/rfd* ${sbindir}/doc* ${sbindir}/serve_image ${sbindir}/recv_image"

# since this package is split, update the original package first.
RDEPENDS_mtd-utils-jffs2 += "${PN} (= ${PV}-${PR})"
RDEPENDS_mtd-utils-misc += "${PN} (= ${PV}-${PR})"
RDEPENDS_mtd-utils-ubifs += "${PN} (= ${PV}-${PR})"

PARALLEL_MAKE = ""

BBCLASSEXTEND = "native"
