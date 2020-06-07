TARGET=rapport

USEBIB=n


ifeq ($(USEBIB),y)
    DEPBIB=%.bbl
else
    DEPBIB=
endif

all: release

release: pdf clean-temp

grammalecte.txt: $(TARGET).txt #grammar correction using grammalecte
	grammalecte-cli --opt_off esp typo nbsp apos  -f $< > grammalecte.txt

%.txt: %.tex
	detex $< > $@

pdf: $(TARGET).pdf 

%.pdf: %.tex %.toc $(DEPBIB) 
	lualatex $<

%.bbl: %.bcf
	biber $<


%.bcf %.toc &: %.tex
	lualatex $<

clean: 
	rm -f *.pdf *.dvi *.log *.toc *.aux *.synctex.gz *.bbl *.blg *.bcf *.run.xml

clean-temp:
	rm -f *.log *.toc *.aux *.bbl *.blg *.bcf *.run.xml
