############################################################
#                                                          #
#     Part of the Creator Live Music Production Suite.     #
# Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012 #
#                                                          #
#                   All rights reserved.                   #
#                                                          #
############################################################

cmake_minimum_required (VERSION 2.8) 
project (creator-live) 

if (NOT DEFINED QNX)
set (NQNX TRUE)
endif (NOT DEFINED QNX)

###################### SUBDIRECTORIES! ######################

add_subdirectory (launcher)

add_subdirectory (3rd-soundtouch) 
#add_subdirectory (3rd-fluidsynth)

add_subdirectory (liblivecore) 
add_subdirectory (liblivewidgets) 

add_subdirectory (app-calf)
add_subdirectory (app-bpm)
add_subdirectory (app-pitch)
add_subdirectory (app-mixer)
add_subdirectory (app-sequencer)
add_subdirectory (app-sampler) 
add_subdirectory (mode-dj)
add_subdirectory (livemidi) 
add_subdirectory (livesf2) 
if (NOT QNX)
add_subdirectory (livevst)
endif (NOT QNX)

add_subdirectory (bridge) 
if (NQNX)
add_subdirectory (pluginTester) 
endif (NQNX)
add_subdirectory (creator-live) 

if (NQNX)
add_subdirectory (test) 
endif (NQNX)

####################### DEPENDENCIES ########################

add_dependencies (livecore 3rd-soundtouch)
add_dependencies (livecore 3rd-fluidsynth)

add_dependencies (app-calf liblivecore)
add_dependencies (app-calf liblivewidgets)

add_dependencies (app-bpm liblivecore)
add_dependencies (app-bpm liblivewidgets)

add_dependencies (app-pitch liblivecore)
add_dependencies (app-pitch liblivewidgets)

add_dependencies (app-mixer liblivecore)
add_dependencies (app-mixer liblivewidgets)

add_dependencies (app-sequencer liblivecore)
add_dependencies (app-sequencer liblivewidgets)

add_dependencies (app-sampler liblivecore)
add_dependencies (app-sampler liblivewidgets)

add_dependencies (livemidi liblivecore)
add_dependencies (livemidi liblivewidgets)
add_dependencies (livesf2 liblivecore)
add_dependencies (livesf2 liblivewidgets)

if (NQNX)

add_dependencies (livevst liblivecore)
add_dependencies (livevst liblivewidgets)
endif (NQNX)

add_dependencies (bridge liblivecore)
add_dependencies (bridge liblivewidgets)
add_dependencies (bridge livemidi)

if (NQNX)
add_dependencies (pluginTest liblivecore)
add_dependencies (pluginTest liblivewidgets)
add_dependencies (pluginTest app-calf)
add_dependencies (pluginTest app-bpm)
add_dependencies (pluginTest app-pitch)
add_dependencies (pluginTest app-calf)
add_dependencies (pluginTest app-mixer)
add_dependencies (pluginTest app-sequencer)
add_dependencies (pluginTest app-sampler)
add_dependencies (pluginTest livemidi)
add_dependencies (pluginTest livesf2)
if (NQNX)
add_dependencies (pluginTest livevst)
endif (NQNX)
endif (NQNX)

add_dependencies (creator-live liblivecore)
add_dependencies (creator-live liblivewidgets)
add_dependencies (creator-live app-calf)
add_dependencies (creator-live app-bpm)
add_dependencies (creator-live app-pitch)
add_dependencies (creator-live app-calf)
add_dependencies (creator-live app-mixer)
add_dependencies (creator-live app-sequencer)
add_dependencies (creator-live app-sampler)
add_dependencies (creator-live livemidi)
add_dependencies (creator-live livesf2)
if (NQNX)
add_dependencies (creator-live livevst)
add_dependencies (run_test pluginTester)
add_dependencies (creator-live run_test)
endif (NQNX)

