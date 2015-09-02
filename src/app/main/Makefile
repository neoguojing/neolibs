#app makefile
export APP_TGT = $(TARGET_PATH)neolibtest
export SOURCES= $(wildcard *.cpp)

TMP_OBJS = $(patsubst %.cpp, %.o,$(SOURCES))
export OBJS = $(addprefix $(OBJECT_PATH),$(TMP_OBJS))

$(APP_TGT):$(TMP_OBJS) 
	@echo "source files:" $(SOURCES)
	@echo "object files:" $(TMP_OBJS)
	$(CXX) $(TMP_OBJS) $(CXXFLAGS) -L $(LIB_TARGET_PATH) -lneolib -o $(APP_TGT)
	#$(CXX) $(TMP_OBJS) $(CXXFLAGS) -L. -lneo -o $(APP_TGT)	   
#sinclude $(SOURCES:.cpp=.d)
sinclude $(addprefix $(OBJECT_PATH),$(SOURCES:.cpp=.d))

include $(BLD_PATH)relation.gen


