
INCLUDEPATH += $$PWD
DEFINES += BEEBEEP_USE_VOICE_CHAT

HEADERS += voicechat/GuiAudioLevel.h \
           voicechat/GuiRecordVoiceMessage.h \
           voicechat/VoicePlayer.h

SOURCES += voicechat/GuiAudioLevel.cpp \
           voicechat/GuiRecordVoiceMessage.cpp \
           voicechat/VoicePlayer.cpp \
           core/CoreVoiceChat.cpp

FORMS += voicechat/GuiRecordVoiceMessage.ui
