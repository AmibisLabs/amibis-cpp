# $Header: $

DESCRIPTION="BIP Service libraries"

basepage="http://www-prima.inrialpes.fr/Vaufreydaz/"

# Homepage, not used by Portage directly but handy for developer reference
HOMEPAGE="$basepage"

# Point to any required sources; these will be automatically downloaded by
# Portage.
SRC_URI="$basepage/${P}.tar.gz"


LICENSE=""


SLOT="0"


KEYWORDS="x86"

IUSE=""

DEPEND=">=dev-libs/libxml2-2.6.19"


S=${WORKDIR}/${P}

src_compile() {
	./configure \
		--host=${CHOST} \
		--prefix=/usr \
		--infodir=/usr/share/info \
		--mandir=/usr/share/man || die "./configure failed"
	

	emake || die "emake failed"
}

src_install(){
	make DESTDIR=${D} install || die "make install failed"
}
