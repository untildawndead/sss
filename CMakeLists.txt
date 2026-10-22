cmake_minimum_required(VERSION 3.22)
project(AbyssalReverb VERSION 1.0.0)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(AbyssalReverb
    COMPANY_NAME "Abyssal Audio"
    BUNDLE_ID "com.abyssalaudio.abyssalreverb"
    PLUGIN_MANUFACTURER_CODE Abys
    PLUGIN_CODE AbRv
    FORMATS VST3 Standalone
    PRODUCT_NAME "Abyssal Reverb"
    DESCRIPTION "Experimental atmospheric reverb"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(AbyssalReverb)

target_sources(AbyssalReverb PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

target_compile_features(AbyssalReverb PRIVATE cxx_std_17)
target_compile_options(AbyssalReverb PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)

target_compile_definitions(AbyssalReverb PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)
target_link_libraries(AbyssalReverb PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags
           juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags
)
