
CXXFLAGS := -std=c++17 -O3 -ftemplate-depth=65536

ifdef SCRIPT_FILE
	CXXFLAGS += -DSCRIPT_FILE=$(SCRIPT_FILE)
else ifdef SCRIPT
	CXXFLAGS += -DSCRIPT='"$(SCRIPT)"'
else
	CXXFLAGS += -DSCRIPT='""'
endif

main:
	$(CXX) $(CXXFLAGS) -o main main.cpp

clean:
	rm main
