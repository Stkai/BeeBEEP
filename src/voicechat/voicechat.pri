
DEFINES += BEEBEEP_USE_VOICE_CHAT

INCLUDEPATH += $$PWD

HEADERS += voicechat/AudioInfo.h \
           voicechat/GuiAudioLevel.h \
           voicechat/GuiRecordVoiceMessage.h

SOURCES += voicechat/AudioInfo.cpp \
           voicechat/GuiAudioLevel.cpp \
           voicechat/GuiRecordVoiceMessage.cpp \
           core/CoreVoiceChat.cpp

FORMS += voicechat/GuiRecordVoiceMessage.ui

