#------------------------------------------------------------------------
# Makefile for objects/degree_days
# ywang254@utk.edu; 19-Mar-2021
#------------------------------------------------------------------------

#PATHOFFSET = path to objects directory
PATHOFFSET = ../..
include ${PATHOFFSET}/build/linux/configure.gcam

OBJS       = degree_days_feedback.o

degree_days_dir: ${OBJS}

-include $(DEPS)

clean:
	rm *.o *.d
