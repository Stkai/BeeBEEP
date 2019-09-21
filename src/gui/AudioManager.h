//////////////////////////////////////////////////////////////////////
//
// This file is part of BeeBEEP.
//
// BeeBEEP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// BeeBEEP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BeeBEEP.  If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_AUDIOMANAGER_H
#define BEEBEEP_AUDIOMANAGER_H

#include "Config.h"
#ifdef BEEBEEP_USE_PHONON4
  #include <phonon/MediaObject>
#endif
class FileInfo;


class AudioManager
{
// Singleton Object
  static AudioManager* mp_instance;

public:
  bool isAudioDeviceAvailable();
  void playBeep();
  void clearBeep();

#if QT_VERSION >= 0x050000
  static QAudioDeviceInfo defaultInputDevice();
  static QAudioFormat defaultAudioFormat();
  static QAudioEncoderSettings defaultAudioEncoderSettings();
#endif
  static QString defaultAudioContainer();
  static QString defaultAudioContainerFilePrefix();
  static QString defaultAudioContainerFileSuffix();
  static QString createDefaultAudioContainerFilename();
  static bool isDefaultAudioContainerFile( const FileInfo& );
  static bool isDefaultAudioContainerFile( const QFileInfo& );

  static AudioManager& instance()
  {
    if( !mp_instance )
      mp_instance = new AudioManager();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      mp_instance->clearBeep();
      delete mp_instance;
      mp_instance = Q_NULLPTR;
    }
  }

protected:
  AudioManager();

private:
#if defined( Q_OS_OS2 )
  void* mp_sound;
#elif defined( BEEBEEP_USE_PHONON4 )
  Phonon::MediaObject *mp_sound;
#else
  QSound* mp_sound;
#endif
};

#endif // BEEBEEP_AUDIOMANAGER_H
