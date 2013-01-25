CXXFLAGS =	-O0 -ggdb -Wall -D_TRACE_LOG_  -fmessage-length=0  -I./ -I../ -I./VTDRRecord -I/usr/local/include 

RECORDS_OBJ = $(patsubst %.cpp, %.o, $(wildcard ./VTDRRecord/*.cpp))

OBJS =		analyzer.o HomeUI.o DataImportUI.o Session.o Worker.o USBFileContentView.o $(RECORDS_OBJ)

LIBS = -L/usr/local/lib/   -lboost_signals -lwt -lwthttp -lwtdbo -lboost_system -lcrypt -lwtdbosqlite3

TARGET =	analyzer


$(TARGET):	$(OBJS) $(RECORD_OBJ)
	echo $(wildcard ./VTDRRecode/*.cpp)
	$(CXX) -o $(TARGET) $^ $(LIBS)

all:	$(TARGET)

test:  test.cpp TraceLog.cpp $(RECORDS_OBJ)
	$(CXX)  -o  test -D_TRACE_LOG_  $(CXXFLAGS) $^

clean:
	rm -f $(OBJS) $(TARGET)
