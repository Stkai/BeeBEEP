
INCLUDEPATH += $$PWD
DEFINES += BEEBEEP_USE_VOICE_CHAT
QTPLUGIN += qtaudio_coreaudio qtmedia_audioengine

HEADERS += voicechat/GuiAudioLevel.h \
           voicechat/GuiRecordVoiceMessage.h \
           voicechat/GuiRecordVoiceMessageSettings.h \
           voicechat/GuiVoicePlayer.h \
           voicechat/VoicePlayer.h

SOURCES += voicechat/GuiAudioLevel.cpp \
           voicechat/GuiRecordVoiceMessage.cpp \
           voicechat/GuiRecordVoiceMessageSettings.cpp \
           voicechat/GuiVoicePlayer.cpp \
           voicechat/VoicePlayer.cpp \
           core/CoreVoiceChat.cpp

FORMS += voicechat/GuiRecordVoiceMessage.ui \
         voicechat/GuiRecordVoiceMessageSettings.ui \
         voicechat/GuiVoicePlayer.ui
