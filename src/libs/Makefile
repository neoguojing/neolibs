#neolib makefile
NEOLIB_TGT = $(LIB_TARGET_PATH)neolib
SOURCES= $(wildcard *.cpp)

TMP_OBJS = $(patsubst %.cpp, %.o,$(SOURCES))
OBJS = $(addprefix $(OBJECT_PATH),$(TMP_OBJS))

#.PHONY :test
#test:
#	@echo $(SOURCES)
#	@echo "object files:" $(TMP_OBJS)
#	@echo "object files:" $(OBJS)

neolib:$(TMP_OBJS) 
	@echo "source files:" $(SOURCES)
	@echo "object files:" $(TMP_OBJS)
	$(CXX) $(TMP_OBJS) $(CXXFLAGS) -o neolib -lpthread
		   
sinclude $(SOURCES:.cpp=.d)

include $(BLD_PATH)relation.gen
#%d: %cpp
#	@echo "create depend"
#	$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; \
#	sed 's,\($*\)\.o[ :]*,\1.o $@ ,g' < $@.$$$$ > $@; \
#	rm -f $@.$$$$
