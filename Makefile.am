EXTRA_DIST = Packages-arch-specific
sysconf_DATA = Packages-arch-specific
SUBDIRS = doc src

ChangeLog:
	git log --date=short --pretty="format:%cd  %aN%n%n%s%n%n%b%n" > ChangeLog

.PHONY: ChangeLog

