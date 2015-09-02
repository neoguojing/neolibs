#neolib makefile
export NEOLIB_TGT = $(LIB_TARGET_PATH)libneolib.so
export SOURCES= $(wildcard *.cpp)

TMP_OBJS = $(patsubst %.cpp, %.o,$(SOURCES))
export OBJS = $(addprefix $(OBJECT_PATH),$(TMP_OBJS))

NEOLIB_TGT:$(TMP_OBJS) 
	@echo "source files:" $(SOURCES)
	@echo "object files:" $(TMP_OBJS)
	#编译程序
	#$(CXX) $(TMP_OBJS) $(CXXFLAGS) -o $(NEOLIB_TGT) -lpthread; \
	#编译动态链接库
	$(CXX) $(TMP_OBJS) $(CXXFLAGS) -fPIC -shared -o $(NEOLIB_TGT) -lpthread ; \
	mv *.o $(OBJECT_PATH)
		   
#sinclude $(SOURCES:.cpp=.d)
sinclude $(addprefix $(OBJECT_PATH),$(SOURCES:.cpp=.d))

include $(BLD_PATH)relation.gen


