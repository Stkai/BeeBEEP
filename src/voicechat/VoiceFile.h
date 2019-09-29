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

#ifndef BEEBEEP_VOICEFILE_H
#define BEEBEEP_VOICEFILE_H

#include "VoiceInfo.h"
class VoiceDecoder;
class VoiceEncoder;

class VoiceFile : public QFile
{
  Q_OBJECT
public:
  VoiceFile( QObject* parent = Q_NULLPTR );
  VoiceFile( const QString&, QObject* parent = Q_NULLPTR );
  ~VoiceFile();

  inline void setAudioFormat( const QAudioFormat& );
  inline const QAudioFormat& audioFormat() const;

  bool open( QFile::OpenMode );
  void close();

signals:
  void levelChanged( qreal );

protected:
  int readWindow( int buffer_size );
  qint64 readData( char* data, qint64 data_size );
  qint64 writeData( const char* data, qint64 data_size );

private:
  VoiceDecoder* mp_decoder;
  VoiceEncoder* mp_encoder;
  VoiceInfo m_info;
  QAudioFormat m_audioFormat;

};

// Inline Functions
inline void VoiceFile::setAudioFormat( const QAudioFormat& new_format ) { m_audioFormat = new_format; }
inline const QAudioFormat& VoiceFile::audioFormat() const { return m_audioFormat; }

#endif // BEEBEEP_VOICEFILE_H
