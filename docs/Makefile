.PHONY: all clean

all: manual.pdf

manual.pdf: manual.ps
	ps2pdf manual.ps

manual.ps: manual.dvi
	dvips -sPAPERSIZE=a4 manual.dvi

manual.dvi: manual.tex resources.bib
	latex manual.tex
	bibtex resources
	latex manual.tex
	latex manual.tex

clean:
	rm -f manual.aux manual.bbl manual.blg manual.dvi manual.log manual.ps manual.pdf