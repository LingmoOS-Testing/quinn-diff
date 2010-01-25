man_MANS = quinn-diff.1

SGMLMANPAGES	= quinn-diff.1.sgml

MANPAGES	= $(patsubst %.sgml, %, $(SGMLMANPAGES))
all: $(MANPAGES)

%: %.sgml
	docbook2man $< > /dev/null

clean:
	rm -f $(MANPAGES) manpage.lings manpage.log manpage.refs

