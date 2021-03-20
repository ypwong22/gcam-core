export GCAM_ROOT=${PROJDIR}/gcam/gcam-core
module load gcc/6.3.0

##############################################################################
# Install Boost
##############################################################################
export BOOST_SRC=${GCAM_ROOT}/build/boost_1_62_0
export BOOST_INSTALL=${GCAM_ROOT}/libs/boost-lib/

cd $BOOST_SRC
./bootstrap.sh --prefix=${BOOST_INSTALL} --with-libraries=system,filesystem
./b2 stage
cp -r boost ${BOOST_INSTALL}
cp -r stage ${BOOST_INSTALL}


##############################################################################
# Install xercesc
##############################################################################
export XERCES_SRC=${GCAM_ROOT}/build/xerces-c-3.2.3
export XERCES_INSTALL=${GCAM_ROOT}/libs/xercesc

cd ${XERCES_SRC}
export CXX='g++'
export CXXFLAGS='-m64'
export CC='gcc'
export CFLAGS='-m64'
export CPP='gcc -E'
export CXXCPP='g++ -E'
./configure --prefix=$XERCES_INSTALL --disable-netaccessor-curl
make install


##############################################################################
# Install ICU because JAVA is disabled
##############################################################################
export ICU_SRC=${PROJDIR}/gcam/icu-release-68-2/icu4c/source
export ICU_INSTALL=${GCAM_ROOT}/libs/icu

cd ${ICU_SRC}
./runConfigureICU Linux/GCC --prefix=${ICU_INSTALL}
make
make check 
make install


##############################################################################
# Install hector
##############################################################################
cd ${GCAM_ROOT}
git submodule init cvs/objects/climate/source/hector
git submodule update cvs/objects/climate/source/hector


##############################################################################
# Install GCAM
##############################################################################
# First disable Java

# In cvs/objects/util/base/include/definitions.h, add
# //! A flag which turns on or off the compilation of the XML database code.
# #ifndef __HAVE_JAVA__
# #define __HAVE_JAVA__ 0
# #endif

# In cvs/objects/reporting/source/xml_db_outputter.cpp, add
# // Whether to write a text file with the contents that are to be inserted
# // into the XML database.
# #define DEBUG_XML_DB 1

# Change the following lines in the configure.gcam file
HAVA_JAVA = 0
LIB             = ${ENVLIBS} $(LIBDIR) -lxerces-c $(JAVALINK) $(HECTOR_LIB) $(TBB_LIB) $(LAPACKLINK) -lm -Wl,-rpath,${GCAM_ROOT}/libs/icu/lib

export CXX=g++
export BOOST_INCLUDE=${GCAM_ROOT}/libs/boost-lib/
export BOOST_LIB=${GCAM_ROOT}/libs/boost-lib/stage/lib
export XERCES_INCLUDE=${GCAM_ROOT}/libs/xercesc/include
export XERCES_LIB=${GCAM_ROOT}/libs/xercesc/lib
cd ${GCAM_ROOT}/cvs/objects/build/linux
make clean # Attention! This clean does not clean the hector folder; Need to do manually. 
make gcam -j 8


##############################################################################
# Test run
##############################################################################
cd ${GCAM_ROOT}/exe
cp configuration_ref.xml config_file.xml
./gcam.exe -C config_file.xml