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

###################### SUBDIRECTORIES! ######################

add_subdirectory (liblivecore) 
add_subdirectory (liblivewidgets) 

add_subdirectory (app-sequencer) 
add_subdirectory (livemidi) 
add_subdirectory (livesf2) 
add_subdirectory (livevst) 

add_subdirectory (bridge) 
add_subdirectory (creator-live) 
#add_subdirectory (pluginTester) 

####################### DEPENDENCIES ########################

add_dependencies (app-sequencer liblivecore)
add_dependencies (app-sequencer liblivewidgets)

add_dependencies (livemidi liblivecore)
add_dependencies (livemidi liblivewidgets)

add_dependencies (livesf2 liblivecore)
add_dependencies (livesf2 liblivewidgets)

add_dependencies (livevst liblivecore)
add_dependencies (livevst liblivewidgets)

add_dependencies (bridge liblivecore)
add_dependencies (bridge liblivewidgets)
add_dependencies (bridge livemidi)

add_dependencies (creator-live liblivecore)
add_dependencies (creator-live liblivewidgets)
add_dependencies (creator-live app-sequencer)
add_dependencies (creator-live livemidi)
add_dependencies (creator-live livesf2)
add_dependencies (creator-live livevst)
