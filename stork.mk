stork_INC_DIRS := $(shell find $(STORK_DIR)/include -type d -not -path "*/.svn*")
stork_INCLUDE  := $(foreach i, $(stork_INC_DIRS), -I$(i))

libmesh_INCLUDE := $(stork_INCLUDE) $(libmesh_INCLUDE)

stork_LIB := $(STORK_DIR)/libstork-$(METHOD).la

stork_APP := $(STORK_DIR)/stork-$(METHOD)

# source files
stork_srcfiles    := $(shell find $(STORK_DIR)/src -name "*.C" -not -name main.C)
stork_csrcfiles   := $(shell find $(STORK_DIR)/src -name "*.c")
stork_fsrcfiles   := $(shell find $(STORK_DIR)/src -name "*.f")
stork_f90srcfiles := $(shell find $(STORK_DIR)/src -name "*.f90")

# object files
stork_objects := $(patsubst %.C, %.$(obj-suffix), $(stork_srcfiles))
stork_objects += $(patsubst %.c, %.$(obj-suffix), $(stork_csrcfiles))
stork_objects += $(patsubst %.f, %.$(obj-suffix), $(stork_fsrcfiles))
stork_objects += $(patsubst %.f90, %.$(obj-suffix), $(stork_f90srcfiles))

# plugin files
stork_plugfiles    := $(shell find $(STORK_DIR)/plugins/ -name "*.C" 2>/dev/null)
stork_cplugfiles   := $(shell find $(STORK_DIR)/plugins/ -name "*.c" 2>/dev/null)
stork_fplugfiles   := $(shell find $(STORK_DIR)/plugins/ -name "*.f" 2>/dev/null)
stork_f90plugfiles := $(shell find $(STORK_DIR)/plugins/ -name "*.f90" 2>/dev/null)

# plugins
stork_plugins := $(patsubst %.C, %-$(METHOD).plugin, $(stork_plugfiles))
stork_plugins += $(patsubst %.c, %-$(METHOD).plugin, $(stork_cplugfiles))
stork_plugins += $(patsubst %.f, %-$(METHOD).plugin, $(stork_fplugfiles))
stork_plugins += $(patsubst %.f90, %-$(METHOD).plugin, $(stork_f90plugfiles))

# stork main
stork_main_src    := $(STORK_DIR)/src/main.C
stork_app_objects := $(patsubst %.C, %.$(obj-suffix), $(stork_main_src))

# dependency files
stork_deps := $(patsubst %.C, %.$(obj-suffix).d, $(stork_srcfiles)) \
              $(patsubst %.c, %.$(obj-suffix).d, $(stork_csrcfiles)) \
              $(patsubst %.C, %.$(obj-suffix).d, $(stork_main_src))

# clang static analyzer files
stork_analyzer := $(patsubst %.C, %.plist.$(obj-suffix), $(stork_srcfiles))

# If building shared libs, make the plugins a dependency, otherwise don't.
ifeq ($(libmesh_shared),yes)
  stork_plugin_deps := $(stork_plugins)
else
  stork_plugin_deps :=
endif

all:: $(stork_LIB)

$(stork_LIB): $(stork_objects) $(stork_plugin_deps)
	@echo "Linking "$@"..."
	@$(libmesh_LIBTOOL) --tag=CXX $(LIBTOOLFLAGS) --mode=link --quiet \
	  $(libmesh_CXX) $(libmesh_CXXFLAGS) -o $@ $(stork_objects) $(libmesh_LIBS) $(libmesh_LDFLAGS) $(EXTERNAL_FLAGS) -rpath $(STORK_DIR)
	@$(libmesh_LIBTOOL) --mode=install --quiet install -c $(stork_LIB) $(STORK_DIR)

# Clang static analyzer
sa:: $(stork_analyzer)

# include STORK dep files
-include $(stork_deps)

# how to build STORK application
ifeq ($(APPLICATION_NAME),stork)
all:: stork

stork: $(stork_APP)

$(stork_APP): $(moose_LIB) $(elk_MODULES) $(stork_LIB) $(stork_app_objects)
	@echo "Linking "$@"..."
	@$(libmesh_LIBTOOL) --tag=CXX $(LIBTOOLFLAGS) --mode=link --quiet \
          $(libmesh_CXX) $(libmesh_CXXFLAGS) -o $@ $(stork_app_objects) $(stork_LIB) $(elk_MODULES) $(moose_LIB) $(libmesh_LIBS) $(libmesh_LDFLAGS) $(ADDITIONAL_LIBS)

endif

#
# Maintenance
#
delete_list := $(stork_APP) $(stork_LIB) $(STORK_DIR)/libstork-$(METHOD).*

cleanall:: 
	make -C $(STORK_DIR) clean 

###############################################################################
# Additional special case targets should be added here
