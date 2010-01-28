man_MANS   = quinn-diff.1
EXTRA_DIST = quinn-diff.1.sgml quinn-diff.1 README.silly-names

SGMLMANPAGES	= quinn-diff.1.sgml

MANPAGES	= $(SGMLMANPAGES:.sgml=)
all: $(MANPAGES)

%: %.sgml
	docbook2man $< > /dev/null

clean-local:
	rm -f $(MANPAGES) manpage.links manpage.log manpage.refs

