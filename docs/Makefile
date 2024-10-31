.PHONY: all clean

all: manual.pdf

manual.pdf: manual.dvi
	dvips -sPAPERSIZE=a4 manual.dvi
	ps2pdf manual.ps

manual.dvi: manual.tex resources.bib
	latex manual.tex
	bibtex manual
	latex manual.tex
	latex manual.tex

clean:
	rm -f manual.aux manual.bbl manual.blg manual.dvi manual.log manual.ps manual.pdf
