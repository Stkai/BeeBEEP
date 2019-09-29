
INCLUDEPATH += $$PWD
DEFINES += BEEBEEP_USE_VOICE_CHAT

HEADERS += voicechat/GuiAudioLevel.h \
           voicechat/GuiRecordVoiceMessage.h \
           voicechat/VoiceCodec.h \
           voicechat/VoiceDecoder.h \
           voicechat/VoiceEncoder.h \
           voicechat/VoiceFile.h \
           voicechat/VoiceInfo.h \
           voicechat/VoicePlayer.h \
           voicechat/VoiceRecorder.h

SOURCES += voicechat/GuiAudioLevel.cpp \
           voicechat/GuiRecordVoiceMessage.cpp \
           voicechat/VoiceCodec.cpp \
           voicechat/VoiceDecoder.cpp \
           voicechat/VoiceEncoder.cpp \
           voicechat/VoiceFile.cpp \
           voicechat/VoiceInfo.cpp \
           voicechat/VoicePlayer.cpp \
           voicechat/VoiceRecorder.cpp \
           core/CoreVoiceChat.cpp

FORMS += voicechat/GuiRecordVoiceMessage.ui
