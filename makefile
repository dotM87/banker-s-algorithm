CXX = g++
CXXFLAGS = -Wall -O2 -std=c++17

SRC = banquero.c
OUT = banquero
TMP = banquero_ascii.c

CHARMAP := $(shell locale charmap)

all: preprocess $(OUT)

preprocess:
ifeq ($(CHARMAP),UTF-8)
	@echo "→ Sistema con codificación UTF-8. Compilando fuente original."
	cp $(SRC) $(TMP)
else
	@echo "⚠ Sistema con codificación $(CHARMAP). Convirtiendo archivo a ASCII..."
	iconv -f utf-8 -t ascii//IGNORE $(SRC) -o $(TMP)
endif

$(OUT): $(TMP)
	$(CXX) $(CXXFLAGS) $(TMP) -o $(OUT)

clean:
	// rm -f $(TMP)
