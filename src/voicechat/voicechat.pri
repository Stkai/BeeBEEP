
INCLUDEPATH += $$PWD
DEFINES += BEEBEEP_USE_VOICE_CHAT

HEADERS += voicechat/AudioCodec.h \
           voicechat/AudioInfo.h \
           voicechat/GuiAudioLevel.h \
           voicechat/GuiRecordVoiceMessage.h \
           voicechat/VoicePlayer.h \
           voicechat/VoiceRecorder.h

SOURCES += voicechat/AudioCodec.cpp \
           voicechat/AudioInfo.cpp \
           voicechat/GuiAudioLevel.cpp \
           voicechat/GuiRecordVoiceMessage.cpp \
           voicechat/VoicePlayer.cpp \
           voicechat/VoiceRecorder.cpp \
           core/CoreVoiceChat.cpp

FORMS += voicechat/GuiRecordVoiceMessage.ui
