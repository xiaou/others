# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/spring/myGits/others/test4MHD

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/spring/myGits/others/test4MHD/build

# Include any dependencies generated for this target.
include CMakeFiles/test4MHD.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test4MHD.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test4MHD.dir/flags.make

CMakeFiles/test4MHD.dir/main.c.o: CMakeFiles/test4MHD.dir/flags.make
CMakeFiles/test4MHD.dir/main.c.o: ../main.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/spring/myGits/others/test4MHD/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/test4MHD.dir/main.c.o"
	g++  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/test4MHD.dir/main.c.o   -c /home/spring/myGits/others/test4MHD/main.c

CMakeFiles/test4MHD.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test4MHD.dir/main.c.i"
	g++  $(C_DEFINES) $(C_FLAGS) -E /home/spring/myGits/others/test4MHD/main.c > CMakeFiles/test4MHD.dir/main.c.i

CMakeFiles/test4MHD.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test4MHD.dir/main.c.s"
	g++  $(C_DEFINES) $(C_FLAGS) -S /home/spring/myGits/others/test4MHD/main.c -o CMakeFiles/test4MHD.dir/main.c.s

CMakeFiles/test4MHD.dir/main.c.o.requires:
.PHONY : CMakeFiles/test4MHD.dir/main.c.o.requires

CMakeFiles/test4MHD.dir/main.c.o.provides: CMakeFiles/test4MHD.dir/main.c.o.requires
	$(MAKE) -f CMakeFiles/test4MHD.dir/build.make CMakeFiles/test4MHD.dir/main.c.o.provides.build
.PHONY : CMakeFiles/test4MHD.dir/main.c.o.provides

CMakeFiles/test4MHD.dir/main.c.o.provides.build: CMakeFiles/test4MHD.dir/main.c.o

# Object files for target test4MHD
test4MHD_OBJECTS = \
"CMakeFiles/test4MHD.dir/main.c.o"

# External object files for target test4MHD
test4MHD_EXTERNAL_OBJECTS =

test4MHD: CMakeFiles/test4MHD.dir/main.c.o
test4MHD: CMakeFiles/test4MHD.dir/build.make
test4MHD: CMakeFiles/test4MHD.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable test4MHD"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test4MHD.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test4MHD.dir/build: test4MHD
.PHONY : CMakeFiles/test4MHD.dir/build

CMakeFiles/test4MHD.dir/requires: CMakeFiles/test4MHD.dir/main.c.o.requires
.PHONY : CMakeFiles/test4MHD.dir/requires

CMakeFiles/test4MHD.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test4MHD.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test4MHD.dir/clean

CMakeFiles/test4MHD.dir/depend:
	cd /home/spring/myGits/others/test4MHD/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/spring/myGits/others/test4MHD /home/spring/myGits/others/test4MHD /home/spring/myGits/others/test4MHD/build /home/spring/myGits/others/test4MHD/build /home/spring/myGits/others/test4MHD/build/CMakeFiles/test4MHD.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test4MHD.dir/depend
