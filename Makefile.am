EXTRA_DIST = Packages-arch-specific
sysconf_DATA = Packages-arch-specific
SUBDIRS = doc src

dist-hook:
	-make ChangeLog
	@set -e; \
	 for file in `find $(srcdir) -type f -name distfiles`; do \
	    dir=`dirname $$file` ; $(mkinstalldirs) $(distdir)/$$dir ; \
	    for i in distfiles `cat $$file` ; do \
		ln $(srcdir)/$$dir/$$i $(distdir)/$$dir/$$i 2> /dev/null \
		|| cp -p $(srcdir)/$$dir/$$i $(distdir)/$$dir/$$i; \
	    done ; \
	done

ChangeLog:
	git log --date=short --pretty="format:%cd  %aN%n%n%s%n%n%b%n" > ChangeLog

.PHONY: ChangeLog

