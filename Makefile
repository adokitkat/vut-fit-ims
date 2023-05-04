CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wpedantic#-fopenmp# -Werror
DEBUG=-g
RELEASE=-O3
LIBS=-lglut -lGLU -lGL

SRCDIR=src/
TMPDIR=tmp/
DOCDIR=doc/
BINDIR=

LOGIN=3_xmlkvy00_xmudry01
OUTPUT=ims

.PHONY: all build build-debug release debug run run-terminal build-run doc zip old

all: build

# Building the program
build: release
build-debug: debug

# Release building
release: $(SRCDIR)main.cpp
	$(CXX) $(CXXFLAGS) $(RELEASE) $(SRCDIR)main.cpp $(LIBS) -o $(BINDIR)$(OUTPUT)

debug: $(SRCDIR)main.cpp
	$(CXX) $(CXXFLAGS) $(DEBUG) $(SRCDIR)main.cpp $(LIBS) -o $(BINDIR)$(OUTPUT)

old: src_old/main.cpp
	$(CXX) $(CXXFLAGS) $(RELEASE) src_old/main.cpp $(LIBS) -o $(BINDIR)$(OUTPUT)

run:
	./ims $(ARGS)

build-run: all run

# Local documentation building
# cd $(DOCDIR) && latexmk -pvc -pdf -interaction=nonstopmode main.tex && latexmk -c
doc: $(DOCDIR)main.tex $(DOCDIR)manual.bib $(DOCDIR)czechiso.bst
	cd $(DOCDIR) && latexmk -pdf main.tex && latexmk -c
	mv $(DOCDIR)main.pdf dokumentacia.pdf

zip: dokumentacia.pdf
	zip -r $(LOGIN).zip Makefile dokumentacia.pdf src/*