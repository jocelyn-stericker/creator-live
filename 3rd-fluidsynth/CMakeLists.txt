# This CMakeLists.txt (C) Joshua Netterfield 2012.

########################## COMMON ##########################

cmake_minimum_required(VERSION 2.8)

list (APPEND livefluidsynth_cpp
#    ./src/fluid_coreaudio.c
    ./src/fluid_mdriver.c
    ./src/fluid_list.c
    ./src/fluid_defsfont.c
    ./src/fluid_coremidi.c
    ./src/fluid_midi.c
    ./src/fluid_sndmgr.c
    ./src/fluid_rev.c
    ./src/fluid_midi_router.c
    ./src/fluid_cmd.c
    ./src/fluid_lash.c
    ./src/fluid_conv.c
    ./src/fluid_winmidi.c
    ./src/fluid_seq.c
    ./src/fluid_dsound.c
    ./src/fluid_portaudio.c
    ./src/fluid_gen.c
    ./src/fluid_synth.c
    ./src/fluid_alsa.c
    ./src/fluid_io.c
    ./src/fluid_dart.c
    ./src/fluidsynth.c
    ./src/fluid_chan.c
    ./src/fluid_midishare.c
    ./src/fluid_event.c
    ./src/fluid_ramsfont.c
    ./src/fluid_tuning.c
    ./src/fluid_settings.c
    ./src/fluid_dsp_simple.c
    ./src/fluid_dsp_float.c
    ./src/fluid_sys.c
    ./src/fluid_voice.c
    ./src/fluid_hash.c
    ./src/fluid_adriver.c
#    ./src/fluid_oss.c
    ./src/fluid_aufile.c
    ./src/fluid_seqbind.c
    ./src/fluid_ladspa.c
#    ./src/fluid_jack.c
    ./src/fluid_chorus.c
    ./src/fluid_mod.c
#    ./src/fluid_dll.c
    ./src/fluid_pulse.c
)

list (APPEND livefluidsynth_qobject
    # not needed.
)

include_directories (
    ./include
)

if (!QNX)
    include_directories ( SYSTEM
        /usr/include # Avoid ugly warnings.
        /usr/include/QtGui # Avoid ugly warnings.
        /usr/include/QtCore # Avoid ugly warnings.
    )
endif (!QNX)

################## PLATFORM-SPECIFIC CODE ##################

if (QNX)
    list (APPEND livefluidsynth_cpp
    )
    list (APPEND livefluidsynth
    )
    list (APPEND livefluidsynth_qobject
    )
else (QNX)
    list (APPEND livefluidsynth_cpp
    )
    list (APPEND livefluidsynth_qobject
    )
endif (QNX)

########################## BUILD! ##########################

add_definitions(-Wall)

find_package (Qt4 REQUIRED)
set (QT_USE_QTNETWORK TRUE)
include (${QT_USE_FILE})

qt4_wrap_cpp (livefluidsynth_moc ${livesoundtouch_qobject})
include_directories (${cmake_binary_dir}
    .
)
add_library (livefluidsynth SHARED
    ${livefluidsynth_cpp}
    ${livefluidsynth_moc}
)

set_target_properties(livefluidsynth PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
