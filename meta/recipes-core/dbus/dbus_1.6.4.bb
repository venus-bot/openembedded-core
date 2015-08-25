include dbus.inc

PR = "${INC_PR}.4"

SRC_URI[md5sum] = "5ec43dc4554cba638917317b2b4f7640"
SRC_URI[sha256sum] = "5fba6b7a415d761a843fb8e0aee72db61cf13057a9ef8cdc795e5d369dc74cf1"

SRC_URI += "file://dbus-daemon-watch.sh"
RDEPENDS_${PN} += "inotify-tools"

do_install_append() {
	install ${WORKDIR}/dbus-daemon-watch.sh ${D}${bindir}/dbus-daemon-watch.sh
}