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
	rm -rf $(LIB_TARGET_PATH)*;\
	rm -rf $(APP_PATH)main/*.o;\
	rm -rf $(APP_PATH)util/*.o;\
	rm -rf $(LIB_PATH)*.o



print: $(LIB_PATH)*.cpp $(LIB_PATH)*.cpp $(APP_PATH)main/*.cpp $(APP_PATH)util/*.cpp 
	#lpr -p $?
	@echo $?
	touch print
	
