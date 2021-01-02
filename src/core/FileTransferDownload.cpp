//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "FileTransferPeer.h"
#include "Protocol.h"
#include "Settings.h"


void FileTransferPeer::sendDownloadData()
{
  if( m_state == FileTransferPeer::Transferring || m_state == FileTransferPeer::Pausing )
    sendDownloadDataConfirmation();
  else
    qWarning() << qPrintable( name() ) << "tries to send download data but it is in invalid state";
}

void FileTransferPeer::sendDownloadRequest()
{
  if( !Settings::instance().isFileExtensionAllowedInFileTransfer( m_fileInfo.suffix() ) )
  {
    qWarning() << qPrintable( name() ) << "has a file extension" << m_fileInfo.suffix() << "not allowed to download";
    cancelTransfer();
    return;
  }

  bool skip_transfer = false;
  QFileInfo existing_file( m_fileInfo.path() );
  if( existing_file.exists() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( name() ) << "has found that the file" << qPrintable( m_fileInfo.path() ) << "already exists";
#endif
    if( Settings::instance().onExistingFileAction() == Settings::SkipExistingFile )
    {
      qDebug() << qPrintable( name() ) << "skips to download existing file" << qPrintable( m_fileInfo.path() );
      m_fileInfo.setStartingPosition( m_fileInfo.size() );
      m_bytesTransferred = m_fileInfo.size();
      skip_transfer = true;
    }
    else if( Settings::instance().onExistingFileAction() == Settings::OverwriteExistingFile )
    {
      qDebug() << qPrintable( name() ) << "is going to overwrite existing file" << qPrintable( m_fileInfo.path() );
      m_fileInfo.setStartingPosition( 0 );
      m_bytesTransferred = 0;
    }
    else if( Settings::instance().onExistingFileAction() == Settings::OverwriteOlderExistingFile )
    {
      QDateTime file_last_modified = existing_file.lastModified();
      if( mp_socket->protocolVersion() >= FILE_TRANSFER_UTC_MODIFIED_DATE_PROTO_VERSION )
        file_last_modified.toUTC();
      if( file_last_modified < m_fileInfo.lastModified() )
      {
        qDebug() << qPrintable( name() ) << "is going to overwrite older existing file" << qPrintable( m_fileInfo.path() );
        m_fileInfo.setStartingPosition( 0 );
        m_bytesTransferred = 0;
      }
      else
      {
        qDebug() << qPrintable( name() ) << "skips to download older existing file" << qPrintable( m_fileInfo.path() );
        m_fileInfo.setStartingPosition( m_fileInfo.size() );
        m_bytesTransferred = m_fileInfo.size();
        skip_transfer = true;
      }
    }
    else
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << qPrintable( name() ) << "is going to overwrite it";
#endif
      m_bytesTransferred = 0;
      m_fileInfo.setStartingPosition( 0 );
    }
  }

  if( !skip_transfer && mp_socket->protocolVersion() >= FILE_TRANSFER_RESUME_PROTO_VERSION )
  {
    QFileInfo file_info( m_file.fileName() );
    if( file_info.exists() && Settings::instance().resumeFileTransfer() )
    {
      m_fileInfo.setStartingPosition( file_info.size() );
      m_bytesTransferred = file_info.size();
    }
    else
      m_fileInfo.setStartingPosition( 0 );
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( name() ) << "sending file request:" << m_fileInfo.id() << m_fileInfo.password() << "-> seek" << m_fileInfo.startingPosition();
#else
  qDebug() << qPrintable( name() ) << "sending file request for" << m_fileInfo.name() << "with starting position" << m_fileInfo.startingPosition();
#endif
  if( mp_socket->sendData( Protocol::instance().fromMessage( Protocol::instance().fileInfoToMessage( m_fileInfo, mp_socket->protocolVersion() ), mp_socket->protocolVersion() ) ) )
  {
    if( skip_transfer )
    {
      QMetaObject::invokeMethod( this, "skipTransfer", Qt::QueuedConnection );
      return;
    }

    if( mp_socket->protocolVersion() < FILE_TRANSFER_2_PROTO_VERSION )
    {
      qWarning() << qPrintable( name() ) << "using an old file download protocol version" << mp_socket->protocolVersion();
      setTransferringState();
    }
    else
      m_state = FileTransferPeer::FileHeader;
  }
  else
    cancelTransfer();
}

void FileTransferPeer::sendDownloadDataConfirmation()
{
  bool transfer_is_about_to_pause = m_state == FileTransferPeer::Pausing;
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( name() ) << "sending" << m_bytesTransferred << "confirmation for" << m_totalBytesTransferred << "/" << m_fileInfo.size() << "bytes" << (transfer_is_about_to_pause ? "(paused)" : "(in progress)");
#endif
  if( !mp_socket->sendData( Protocol::instance().fileTransferBytesArrivedConfirmation( mp_socket->protocolVersion(), m_bytesTransferred, m_totalBytesTransferred, transfer_is_about_to_pause ) ) )
  {
    cancelTransfer();
    return;
  }

  if( transfer_is_about_to_pause )
    setTransferPaused();
}

void FileTransferPeer::checkDownloadData( const QByteArray& byte_array )
{
  if( m_state == FileTransferPeer::Request )
  {
    qWarning() << qPrintable( name() ) << "has found an invalid Request state in FileTransferPeer::checkDownloadData(...)";
    cancelTransfer();
    return;
  }

  if( m_state == FileTransferPeer::FileHeader )
  {
    Message file_header_message = Protocol::instance().toMessage( byte_array, mp_socket->protocolVersion() );
    if( !file_header_message.isValid() )
    {
      setError( tr( "invalid file header" ) );
      return;
    }

    FileInfo file_header = Protocol::instance().fileInfoFromMessage( file_header_message, mp_socket->protocolVersion() );
    if( m_bytesTransferred > 0 && file_header.startingPosition() != m_bytesTransferred )
      m_bytesTransferred = 0;
    m_totalBytesTransferred = m_bytesTransferred;

    m_fileInfo.setSize( file_header.size() );
    if( file_header.lastModified().isValid() )
      m_fileInfo.setLastModified( file_header.lastModified() );
    setTransferringState();
    sendTransferData();
    return;
  }

  if( m_state != FileTransferPeer::Transferring && m_state != FileTransferPeer::Pausing )
  {
    qWarning() << qPrintable( name() ) << "tries to check data with invalid state" << m_state;
    return;
  }

  m_bytesTransferred = static_cast<FileSizeType>( byte_array.size() );
  m_totalBytesTransferred += m_bytesTransferred;

  sendTransferData(); // send to upload client that data is arrived

  if( m_bytesTransferred > 0 )
  {
    if( !m_file.isOpen() )
    {
      if( !m_file.open( QIODevice::WriteOnly | QIODevice::Append ) )
      {
        setError( tr( "Unable to open file %1" ).arg( m_file.fileName() ) );
        return;
      }
    }

    if( m_file.write( byte_array ) != static_cast<int>( m_bytesTransferred ) )
    {
      setError( tr( "Unable to write in the file %1" ).arg( m_file.fileName() ) );
      return;
    }

    showProgress();
  }

  if( m_totalBytesTransferred > m_fileInfo.size() )
    setError( tr( "%1 bytes downloaded but the file size is only %2 bytes" ).arg( m_totalBytesTransferred ).arg( m_fileInfo.size() ) );

  if( m_totalBytesTransferred == m_fileInfo.size() )
    setTransferCompleted();
}

QString FileTransferPeer::temporaryFilePath() const
{
  return QString( "%1/%2.%3" ).arg( Settings::instance().cacheFolder(), m_fileInfo.fileHash(), Settings::instance().partiallyDownloadedFileExtension() );
}

bool FileTransferPeer::removePartiallyDownloadedFile()
{
  if( !isDownload() || isTransferCompleted() )
    return false;
  if( m_file.exists() && m_file.fileName().endsWith( QString( ".%1" ).arg( Settings::instance().partiallyDownloadedFileExtension() ) ) && m_file.remove() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Removed partially downloaded file" << qPrintable( m_file.fileName() );
#endif
    return true;
  }
  qWarning() << "Unable to remove partially downloaded file" << qPrintable( m_file.fileName() );
  return false;
}
