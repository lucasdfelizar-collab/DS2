cmake_minimum_required(VERSION 3.22)
project(12GAUGE_DS2 STYLE VERSION 1.0.0)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(12GAUGE_DS2
    COMPANY_NAME "Lucasdfelizar Audio"
    BUNDLE_ID "com.lucasdfelizar.12gaugeds2"
    PLUGIN_MANUFACTURER_CODE Lcdf
    PLUGIN_CODE Gds2
    FORMATS VST3 Standalone
    PRODUCT_NAME "12 Gauge DS-2")

juce_generate_juce_header(12GAUGE_DS2)
target_sources(12GAUGE_DS2 PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)
target_compile_features(12GAUGE_DS2 PRIVATE cxx_std_17)
target_compile_definitions(12GAUGE_DS2 PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(12GAUGE_DS2 PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags
           juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags)
