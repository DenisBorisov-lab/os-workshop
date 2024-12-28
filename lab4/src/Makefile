CXX = g++

# fPIC - создание позиционно-независимого кода для динамических библиотек
CXXFLAGS = -std=c++11 -Wall -Wextra -fPIC -shared -O2
APPFLAGS = -std=c++11 -Wall -Wextra -O2
# флаг, указывающий на необходимость подключения динамических бибилиотек
LDFLAGS = -ldl

# RPATH настройки (информация о том где искать динамические библиотеки)
RPATH = -Wl,-rpath,\$$ORIGIN

# Динамические библиотеки
LIBS = libSinIntegral_Rect.so libSinIntegral_Trapezoid.so libSort_Bubble.so libSort_Quick.so

all: $(LIBS) TestProgram1 TestProgram2

libSinIntegral_Rect.so: SinIntegral_Rect.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

libSinIntegral_Trapezoid.so: SinIntegral_Trapezoid.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

libSort_Bubble.so: Sort_Bubble.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

libSort_Quick.so: Sort_Quick.cpp
	$(CXX) $(CXXFLAGS) -o $@ Sort_Quick.cpp

TestProgram1: TestProgram1.cpp
	$(CXX) $(APPFLAGS) -o $@ TestProgram1.cpp -L. -lSinIntegral_Rect -lSort_Bubble $(RPATH)

TestProgram2: TestProgram2.cpp
	$(CXX) $(APPFLAGS) -o $@ TestProgram2.cpp $(LDFLAGS) $(RPATH)

clean:
	rm -f $(LIBS) TestProgram1 TestProgram2
