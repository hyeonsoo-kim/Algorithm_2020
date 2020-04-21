CC = gcc
CXXSTD = c++11
CXXLIB = stdc++
FLAG = -std=$(CXXSTD) -l$(CXXLIB) -lm
TEX = xelatex
all: pa01-1 pa01-2

pa01-1: PA01-1.cpp
	@echo "[*] Programming Assignment 01-1 : Compile..."
	@$(CC) PA01-1.cpp -o pa01-1 $(FLAG)

pa01-2: PA01-2.cpp
	@echo "[*] Programming Assignment 01-2 : Compile..."
	@$(CC) PA01-2.cpp -o pa01-2 $(FLAG)

report: PA01-Report.tex 
	@echo "[*] Generating Report"
	@$(TEX) PA01-Report.tex 

clean:
	@rm -rf *.exe *.log *.out pa01-1 pa01-2 PA01-Report.pdf
