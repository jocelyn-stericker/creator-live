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
if (NQNX)
add_subdirectory (livesf2) 
endif (NQNX)
if (NOT WIN32)
add_subdirectory (livevst)
endif (NOT WIN32)

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

if (NOT WIN32)
add_dependencies (app-bpm liblivecore)
add_dependencies (app-bpm liblivewidgets)

add_dependencies (app-pitch liblivecore)
add_dependencies (app-pitch liblivewidgets)
endif (NOT WIN32)

add_dependencies (app-mixer liblivecore)
add_dependencies (app-mixer liblivewidgets)

add_dependencies (app-sequencer liblivecore)
add_dependencies (app-sequencer liblivewidgets)

add_dependencies (app-sampler liblivecore)
add_dependencies (app-sampler liblivewidgets)

add_dependencies (livemidi liblivecore)
add_dependencies (livemidi liblivewidgets)

if (NQNX)
add_dependencies (livesf2 liblivecore)
add_dependencies (livesf2 liblivewidgets)
endif (NQNX)

if (NOT WIN32)
add_dependencies (livevst liblivecore)
add_dependencies (livevst liblivewidgets)
endif (NOT WIN32)

add_dependencies (bridge liblivecore)
add_dependencies (bridge liblivewidgets)
add_dependencies (bridge livemidi)

if (NQNX)
add_dependencies (pluginTester liblivecore)
add_dependencies (pluginTester liblivewidgets)
add_dependencies (pluginTester app-calf)
if (NOT WIN32)
add_dependencies (pluginTester app-bpm)
add_dependencies (pluginTester app-pitch)
endif (NOT WIN32)
add_dependencies (pluginTester app-calf)
add_dependencies (pluginTester app-mixer)
add_dependencies (pluginTester app-sequencer)
add_dependencies (pluginTester app-sampler)
add_dependencies (pluginTester livemidi)
if (NQNX)
add_dependencies (pluginTester livesf2)
endif (NQNX)
add_dependencies (pluginTester livevst)
endif (NQNX)

add_dependencies (creator-live liblivecore)
add_dependencies (creator-live liblivewidgets)
add_dependencies (creator-live app-calf)
if (NOT WIN32)
add_dependencies (creator-live app-bpm)
add_dependencies (creator-live app-pitch)
endif (NOT WIN32)
add_dependencies (creator-live app-calf)
add_dependencies (creator-live app-mixer)
add_dependencies (creator-live app-sequencer)
add_dependencies (creator-live app-sampler)
add_dependencies (creator-live livemidi)
if (NQNX)
add_dependencies (creator-live livesf2)
endif (NQNX)
if (NOT WIN32)
add_dependencies (creator-live livevst)
endif (NOT WIN32)

if (NQNX)
add_dependencies (test creator-live)
endif (NQNX)
