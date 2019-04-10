###############################################################################
################### MOOSE Application Standard Makefile #######################
###############################################################################
#
# Optional Environment variables
# MOOSE_DIR        - Root directory of the MOOSE project
#
###############################################################################
# Use the MOOSE submodule if it exists and MOOSE_DIR is not set
MOOSE_SUBMODULE    := $(CURDIR)/moose
ifneq ($(wildcard $(MOOSE_SUBMODULE)/framework/Makefile),)
  MOOSE_DIR        ?= $(MOOSE_SUBMODULE)
else
  MOOSE_DIR        ?= $(shell dirname `pwd`)/moose
endif

# framework
FRAMEWORK_DIR      := $(MOOSE_DIR)/framework
include $(FRAMEWORK_DIR)/build.mk
include $(FRAMEWORK_DIR)/moose.mk

################################## MODULES ####################################
ALL_MODULES := no
PHASE_FIELD := yes
NAVIER_STOKES := yes
include $(MOOSE_DIR)/modules/modules.mk
###############################################################################

# Use the SQUIRREL submodule if it exists and SQUIRREL_DIR is not set
SQUIRREL_SUBMODULE    := $(CURDIR)/squirrel
ifneq ($(wildcard $(SQUIRREL_SUBMODULE)/Makefile),)
  SQUIRREL_DIR        ?= $(SQUIRREL_SUBMODULE)
else
  SQUIRREL_DIR        ?= $(shell dirname `pwd`)/squirrel
endif

# squirrel
APPLICATION_DIR    := $(SQUIRREL_DIR)
APPLICATION_NAME   := squirrel
include            $(FRAMEWORK_DIR)/app.mk

# dep apps
APPLICATION_DIR    := $(CURDIR)
APPLICATION_NAME   := moltres
BUILD_EXEC         := yes
DEP_APPS           := $(shell $(FRAMEWORK_DIR)/scripts/find_dep_apps.py $(APPLICATION_NAME))
ADDITIONAL_INCLUDES = -I/opt/moose/miniconda/include
EXTERNAL_FLAGS = -I/opt/moose/miniconda/include -D_FORTIFY_SOURCE=2 -O2 -fvisibility-inlines-hidden -std=c++17 -fmessage-length=0 -march=nocona -mtune=haswell -ftree-vectorize -fPIC -fstack-protector-strong -fno-plt -O2 -pipe -I/opt/moose/miniconda/include   -L/opt/moose/miniconda/lib /opt/moose/miniconda/lib/libhdf5_hl_cpp.so /opt/moose/miniconda/lib/libhdf5_cpp.so /opt/moose/miniconda/lib/libhdf5_hl.so /opt/moose/miniconda/lib/libhdf5.so -L/opt/moose/miniconda/lib -Wl,-O2 -Wl,--sort-common -Wl,--as-needed -Wl,-z,relro -Wl,-z,now -Wl,-rpath,/opt/moose/miniconda/lib -L/opt/moose/miniconda/lib -lrt -lpthread -lz -ldl -lm -Wl,-rpath -Wl,/opt/moose/miniconda/lib
include            $(FRAMEWORK_DIR)/app.mk

###############################################################################
# Additional special case targets should be added here
