CXX=g++
CXXFLAGS=-g -Wall
CPPFLAGS=`primacontrol-config --cflags`
LDFLAGS=`primacontrol-config --libs`

all : testregister testclient testRegisterFromXml

testclient: testclient.o
	${CXX} -o testclient testclient.o ${LDFLAGS}	

testregister : testregister.o
	${CXX} -o testregister testregister.o ${LDFLAGS}
testRegisterFromXml : testRegisterFromXml.o
	${CXX} -o testRegisterFromXml testRegisterFromXml.o ${LDFLAGS}

%.o: %.cpp
	$(CXX) -c ${CXXFLAGS} ${CPPFLAGS} -o $@ $<

clean : 
	rm *.o *~
	rm testregister
	rm testclient
	rm testRegisterFromXml
