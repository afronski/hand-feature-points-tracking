#!/usr/bin/make -f
TEXF    = MainDocument
LOG     = yes
COVER   = cover
GOAL    = pdf

LATEX     = latex
BIBTEX    = bibtex
PDFLATEX  = pdflatex
PS2PS     = pstops
PSSELECT  = psselect
PS2PDF    = ps2pdf
GS        = gs
XPDF      = xpdf -fullscreen
GIT       = git

RAWNAME := $(shell echo "$(TEXF)" |sed -e 's/\.[^\.]\+$$//')

DVIF  := $(RAWNAME).dvi
PSF   := $(RAWNAME).ps
PDFF  := $(RAWNAME).pdf
LOGF  := $(RAWNAME).log

RERUN   = "(There were undefined references|Rerun to get (cross-references|the bars) right)"
BIB     = "No file.*\.bbl|Citation.*undefined"

CLEANF    := *.aux *.blg *.out *.ilg *.ind *.bbl *.brf *.toc *.lof *.lot *.lop
MRPROPERF := *.ps *.dvi *.pdf *.log

ifeq ("$(LOG)", "no")
	CLEANF  += *.log
endif

all: $(GOAL)

dvi: rmdvi _dvi
ps:   rmps _ps rmdvi
pdf:  rmpdf _pdf

bib: phd.bib
_dvi: skrot $(DVIF) clean
_ps:  skrot $(PSF)  clean
_pdf: imepspdf skrot $(PDFF) clean

rmdvi:
	-rm -f $(DVIF)
rmps:
	-rm -f $(PSF)
rmpdf:
	-rm -f $(PDFF)

cph:
	@rm -f phd.bib
phd.bib:
	@cat [A-Z]*.bib > phd.bib
	@cat bib.d/* >>phd.bib

pspdf: ps
	$(PS2PDF) $(PSF)
	-rm -f $(PSF)
psgspdf: ps
	@cat $(PSF) | $(GS) -sDEVICE=pdfwrite -sOutputFile=$(PDFF) -dNOPAUSE -q -
	-rm -f $(PSF)

%.dvi: %.tex
	$(LATEX) $<
	egrep -c $(BIB) $(LOGF) >/dev/null && $(BIBTEX) $(RAWNAME) && $(LATEX) $< ; true
	egrep -c $(RERUN) $(LOGF)  >/dev/null && $(LATEX) $< ; true
	egrep -c $(RERUN) $(LOGF)  >/dev/null && $(LATEX) $< ; true
	egrep -i "(Reference|Citation).*undefined" $(LOGF) ; true

%.ps:   %.dvi
	dvips $< -o $(PSF)

%.pdf: %.tex
	$(PDFLATEX) $<
	egrep -c $(BIB) $(LOGF) >/dev/null && $(BIBTEX) $(RAWNAME) && $(PDFLATEX) $< ; true
	egrep -c $(RERUN) $(LOGF) >/dev/null && $(PDFLATEX) $< ; true
	egrep -c $(RERUN) $(LOGF) >/dev/null && $(PDFLATEX) $< ; true
	egrep -i "(Reference|Citation).*undefined" $(LOGF) ; true

skrot:
	-scripts/genskrot
imepspdf:
	-scripts/imeps
clean:
	-rm -f $(CLEANF)
mrproper: clean
	-rm -f $(MRPROPERF)
bak:
	-scripts/bak
cover:
	-scripts/preparecover $(TEXF) $(COVER)
show:
	@$(XPDF) $(PDFF)

gitr:
	@$(GIT) add *.tex *.bib *.sty Makefile
