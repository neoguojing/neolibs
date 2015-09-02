include tool.def
include mkpath.def
include global.def


all : $(DEPEND_LIST)
	@echo $(NEO_ROOT)

include mklist
	
.PHONY :clean
clean :
	rm -rf $(OBJECT_PATH)*.o; \
	rm -rf $(OBJECT_PATH)*.d; \
	rm -rf $(TARGET_PATH)*; \
	rm -rf $(LIB_TARGET_PATH)*



print: $(LIB_PATH)*.cpp $(LIB_PATH)*.cpp $(APP_PATH)main/*.cpp $(APP_PATH)util/*.cpp 
	#lpr -p $?
	@echo $?
	touch print
	
