{ This file is part of classwork.pp, a library of basic and useful classes.

  This program is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 3 of the License or, at your
  option, any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABLITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU General Public License along
  with this program.  If not, see <http://www.gnu.org/licenses/>.
}
{$IFDEF ReadInterface}
{ A file stream.

  It is unlikely that you will directly instantiate this class, since it is
  incapable of performing read or write operations; these operations are instead
  delegated to one of its descendants: @link(AFileInputStream) or
  @link(AFileOutputStream).
}
type AFileStream = class(AStream, CanPrint)
  protected
    /// Stores the handle used to represent the file
    myFile: file;
    /// Stores the name of the file
    myFileName: string;
    /// Indicates whether or not the file handle is valid
    myHandleIsValid: boolean;

  public
    { Initializes the instance. }
    function init: boolean; override;

    { Destroys the instance.

      This routine is called by @link(TObject.free).  It calls
      @link(AFileStream.close) automatically, to ensure that the file stream
      is closed and all buffers are flushed to disk before the stream is
      destroyed.
    }
    destructor destroy; override;

    { Reads @code(count) bytes from the stream into @code(dest) and returns
      the total number of bytes read.

      It is the caller's responsibility to ensure that @code(dest) has
      space sufficient for storing all of the data that will be read from the
      stream; failure to do so will likely destabilize your program and crash
      your code.

      This method may throw an exception if the read fails, but it should NOT
      throw an exception if the end of the stream is encountered; instead, it
      should simply return the total number of bytes that were read before the
      end of the stream was reached.

      As implemented in @link(AFileStream), this routine does nothing and
      returns zero (0).  It is for those descendants that support reading from
      a file to implement this method; see @link(AFileInputStream.read).

      @return(The total number of bytes read from the stream.  If the end of
      the stream is reached while reading, this value may be less than
      @code(count).)
    }
    function read(const count: TStreamIOSize; var dest): TStreamIOSize;
      override;

    { Writes @code(count) bytes from @code(source) to the stream and returns
      the total number of bytes written.

      It is the caller's responsibility to ensure that @code(source) has at
      least @code(count) bytes ready to be written; failure to do so will
      likely cause your code to crash.

      This method may throw an exception if the write operation fails.

      As implemented in @link(AFileStream), this routine does nothing and
      returns zero (0).  It is for those descendant that support writing to
      a file to implement this method; see @link(AFileOutputStream.write).

      @return(The total number of bytes written to the stream.)
    }
    function write(const count: TStreamIOSize; const source): TStreamIOSize;
      override;

    { Flushes any buffered data to the underlying file and then closes the file.

      It is unlikely that you will call this routine directly; it is called
      automatically when the file stream is destroyed.
    }
    procedure close; virtual;

    { Determines whether or not the end of the stream has been reached.

      This method calls @code(System.eof) on the underlying file and returns
      @true if the end of the file has been reached, and @false otherwise.
    }
    function hasEnded: boolean; override;

    { Returns the name of the file being streamed. }
    function name: string; override;

    // CanPrint implementation -------------------------------------------------
    { Constructs and returns a string representation of the class.

      The base implementation of this method for @link(AFileStream) simply
      returns the name of the underlying file represented by the stream;
      descendant classes may override this routine to return something different
      instead.
    }
    function toString: string; override;

    { Prints a string representation of the class to the specified stream.

      The base implementation of this method for @link(AFileStream) calls
      @link(AFileStream.toString) and prints the resulting representation to
      @code(Dest).  Descendant classes may override this routine to produce
      different behavior.

      @return(The total number of bytes written to @code(Dest).)
    }
    function printTo(const Dest: AStream;
      prefix: AnsiString = '';
      suffix: AnsiString = ''): TStreamIOSize; virtual;

  // Properties ----------------------------------------------------------------
    { Retrieves the current position within the stream.  This is the location
      at which the next read or write operation on the stream will take place.

      The value returned by this routine is always measured in bytes from the
      beginning of the stream.  The first byte in the stream is always at
      offset zero (0).
    }
    function position: TStreamOffset; override;

    { Sets the current position within the stream.  This is the location at
      which the next read or write operation on the stream will take place.

      @code(newPosition) may be either a positive or negative value.  If
      positive, then it represents a position in the stream relative to the
      beginning of the stream.  The first byte in the stream is always at
      offset zero (0).

      If @code(newPosition) is a negative value, then it represents a position
      in the stream relative to the end of the stream.  In this case, the last
      byte in the stream is always located at offset -1.

      This method should not throw an exception if the underlying mechanism
      does not support repositioning; it should instead fail silently and
      return the current, unchanged position within the stream.

      @return(The previous position within the stream.)
    }
    function setPosition(newPosition: TStreamOffset): TStreamOffset; override;

    { Retrieves the current length of the stream; this is the current size of
      the underlying file, measured in bytes.

      For streams that support write operations, this value will change as
      more data is written to the stream.
    }
    function length: TStreamLength; override;

    { Retrieve the handle of the underlying file.

      In the base implementation of @link(AFileStream), the handle returned
      by this routine will simply be a reference to the Pascal @code(file)
      object that represents the file on disk.
    }
    function handle: pointer; virtual;

    { Retrieve the name of the file being streamed. }
    function fileName: string; virtual;

    { Return whether or not the file handle returned by a call to
      @link(AFileStream.handle) is valid.

      If the attempt to open or create the file named by
      @link(AFileStream.fileName) fails, then this routine will return @false.
      Otherwise, it will return @true.
    }
    function handleIsValid: boolean; virtual;
end;

{ This class can be used for binary input from a file that can be accessed by
  the underlying file system.
}
type AFileInputStream = class(AFileStream)
  public
    { Construct a new stream instance that will read from the file named
      by @code(thisName).

      @code(thisName) may represent a relative or absolute path to the file
      to be read.  If the file does not exist or cannot be accessed for
      reading, then an exception is raised.

      @raises(AFileStreamOpenError if the file named by @code(thisName) cannot be
      accessed for reading.)
    }
    constructor fromFileNamed(const thisName: string); virtual;

    { Read @code(count) bytes from the stream and store them in @code(dest).
      See @link(AStream.read) for more information on this method.

      @return(The total number of bytes read.)

      @raises(AFileStreamReadError if an error is encountered while attempting
      to read from the underlying file.)
    }
    function read(const count: TStreamIOSize; var dest): TStreamIOSize;
      override;
end;

{ This class can be used for binary output to any file that is accessible to
  the underlying file system.
}
type AFileOutputStream = class(AFileStream)
  public
    { Construct a new stream instance that will write to the file named by
      @code(thisName).

      @code(thisName) may represent a relative or absolute path to the file
      to be written.  If the file does not exist, it will be created.  If the
      file exists and @code(overwrite) is @true, then the contents of the
      file will be deleted before writing begins; otherwise, new data will be
      appended to the end of the file.

      If the file cannot be accessed for writing, then an exception is
      raised.

      @raises(AFileStreamOpenError if the file named by @code(thisName) cannot
      be accessed for writing.)
    }
    constructor toFileNamed(const thisName: string;
      const overwrite: boolean = false); virtual;

    { Write @code(count) bytes from @code(source) to the stream.  See
      @link(AStream.write) for more information on this method.

      @return(The total number of bytes written).

      @raises(AFileStreamWriteError if an error is encountered while attempting
      to write to the underlying file.)
    }
    function write(const count: TStreamIOSize; const source): TStreamIOSize;
      override;
end;

{ An instance of this exception class is raised if the constructor for
  @link(AFileInputStream) fails to open the specified file for reading, or if
  the constructor for @link(AFileOutputStream) fails to open the specified
  file for writing.
}
type AFileStreamOpenError = class(AnException);

{ An instance of this exception class is raised if @link(AFileInputStream.read)
  fails to retrieve data from the underlying file.
}
type AFileStreamReadError = class(AnException);

{ An instance of this exception class is raised if @link(AFileOuputStream.write)
  fails to write data to the underlying file.
}
type AFileStreamWriteError = class(AnException);

// Returns an error message string for the specified i/o result code
function ioMessage(const ioCode: longword): string;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string controls the format of the message displayed when the
    constructor for @link(AFileInputStream) or its descendants fails to open a
    specified file for reading.  It is passed to the instance of
    @link(AFileStreamOpenError) that is raised by the constructor.

    The first string placeholder is filled with the name of the file as passed
    to the constructor.  The second string placeholder will contain a string
    indicating what type of error occurred; this will be one of the I/O
    status messages, such as @link(ioFileNotFound).
  }
  fserrOpenRead = 'failed to open "%s" for reading because %s';

  { This string controls the format of the message displayed when the
    constructor for @link(AFileOutputStream) or its descendants fails to open
    a specified file for writing.  It is passed to the instance of
    @link(AFileStreamOpenError) that is raised by the constructor.

    The first string placeholder is filled with the name of the file as passed
    to the constructor.  The second string placeholder will contain a string
    indicating what type of error occurred; this will be one of the I/O status
    messages, such as @link(ioAccessDenied).
  }
  fserrOpenWrite = 'failed to open "%s" for writing because %s';

  { This string is used to raise an exception in the event that the constructors
    for @link(AFileInputStream) or @link(AFileOutputStream) are passed an
    empty string as the name of a file.  It is passed to the instance of
    @link(AFileStreamOpenError) that is raised by these constructors.
  }
  fserrInvalidFileName = 'an invalid file name was provided to the stream ' +
    'handler';

  { This string controls the format of the message displayed when a call to
    @link(AFileInputStream.read) fails for some reason.  It is passed to the
    instance of @link(AFileStreamReadError) that is raised by the method in
    such cases.

    The first string placeholder is filled with the name of the file being
    read, as obtained by a call to @link(AFileStream.fileName).  The second
    string placeholder is filled by a string indicating the type of error that
    occurred; this will be one of the I/O status messages, such as
    @link(ioWriteOnly).
  }
  fserrReading = 'failed to read from "%s" because %s';

  { This string controls the format of the message displayed when a call to
    @link(AFileOutputStream.write) fails for some reason.  It is passed to the
    instance of @link(AFileStreamWriteError) that is raised by the method in
    such cases.

    The first string placeholder is filled with the name of the file being
    read, as obtained by a call to @link(AFileStream.fileName).  The second
    string placeholder is filled by a string indicating the type of error that
    occurred; this will be one of the I/O status messages, such as
    @link(ioReadOnly).
  }
  fserrWriting = 'failed to write to "%s" because %s';

  // I/O status messages
  ioSuccess = 'the operation was successful';
  ioFileNotFound = 'the specified file was not found';
  ioPathNotFound = 'the specified path was not found';
  ioTooManyFiles = 'there are already too many open files';
  ioAccessDenied = 'access to the file was denied';
  ioInvalidHandle = 'an invalid file handle was specified';
  ioInvalidOperation = 'an invalid operation was attempted';
  ioDriveNotFound = 'the specified drive was not found';
  ioRemovingDirectory = 'the current directory could not be removed';
  ioRenamingFailed = 'the specified file could not be renamed';
  ioDriveReadFailure = 'the drive could not be read';
  ioDriveWriteFailure = 'writing to the drive failed';
  ioNoFileHandle = 'a handle must first be assigned to the file';
  ioFileNotOpen = 'the file must first be opened';
  ioWriteOnly = 'the file was opened for writing only';
  ioReadOnly = 'the file was opened for reading only';
  ioInvalidNumber = 'an invalid number was encountered';
  ioDriveProtected = 'the drive is protected against writes';
  ioUnknownDevice = 'the specified device is unknown';
  ioDriveUnavailable = 'the drive is temporarily unable to handle the request';
  ioUnknownCommand = 'the command is unrecognized';
  ioDataCheckFailed = 'the data integrity check failed';
  ioInvalidDrive = 'the drive specified is invalid';
  ioSeekFailure = 'the proper location on disk was not found';
  ioIncorrectMedia = 'incorrect media is loaded in the drive';
  ioSectorNotFound = 'the appropriate sector on disk was not found';
  ioPrinterPaper = 'the printer is out of paper';
  ioWriteError = 'writing to the device failed';
  ioReadError = 'reading from the device failed';
  ioHardwareFailure = 'a hardware failure occurred';
  ioUnknown = 'an unknown I/O error occurred';
{$ENDIF}
{$IFDEF ReadImplementation}
// Returns an error message string for the specified i/o result code
function ioMessage(const ioCode: longword): string;

begin
  case ioCode of
    0: result := ioSuccess;
    2: result := ioFileNotFound;
    3: result := ioPathNotFound;
    4: result := ioTooManyFiles;
    5: result := ioAccessDenied;
    6: result := ioInvalidHandle;
    12: result := ioInvalidOperation;
    15: result := ioDriveNotFound;
    16: result := ioRemovingDirectory;
    17: result := ioRenamingFailed;
    100: result := ioDriveReadFailure;
    101: result := ioDriveWriteFailure;
    102: result := ioNoFileHandle;
    103: result := ioFileNotOpen;
    104: result := ioWriteOnly;
    105: result := ioReadOnly;
    106: result := ioInvalidNumber;
    150: result := ioDriveProtected;
    151: result := ioUnknownDevice;
    152: result := ioDriveUnavailable;
    153: result := ioUnknownCommand;
    154: result := ioDataCheckFailed;
    155: result := ioInvalidDrive;
    156: result := ioSeekFailure;
    157: result := ioIncorrectMedia;
    158: result := ioSectorNotFound;
    159: result := ioPrinterPaper;
    160: result := ioWriteError;
    161: result := ioReadError;
    162: result := ioHardwareFailure;

  else
    result := ioUnknown;
  end;
end;

// AFileStream *****************************************************************
{* Initializes a new instance of AFileStream.  This routine sets
   AFileStream.fileName to an empty string and sets an internal flag indicating
   whether or not a file was successfully opened to #false.  It is unlikely that
   this routine will be called directly, as AFileStream is designed to be an
   abstract ancestor for more specific file streams, such as AFileInputStream
   and AFileOutputStream.

   \return #true if a new instance of AFileStream was successfully initialized;
   #false if not.  In the base implementation, this routine returns the same
   value as AnObject.init (always #true).
*}
function AFileStream.init: boolean;

begin
  myFileName := '';
  myHandleIsValid := false;
  result := inherited init;
end;

{* Destroys an instance of AFileStream.  The destructor first calls
   AFileStream.close to ensure that the associated file is closed and all
   buffered data is flushed.
*}
destructor AFileStream.destroy;

begin
  // Close the file
  Self.close;
  inherited destroy;
end;

{* This routine is designed to be overridden by descendant classes.  It is
   unlikely that it will be directly called, as it always returns zero (0) in
   the base implementation.

   \return The number of bytes read from the stream.  In the base
   implementation of AFileStream, this value is always zero (0).
*}
function AFileStream.read(const count: TStreamIOSize; var dest): TStreamIOSize;

begin
  // Does nothing here; this is for descendant classes to implement
  result := 0;
end;

{* This routine is designed to be overridden by descendant classes.  It is
   unlikely that it will be directly called, as it always returns zero (0) in
   the base implementation.

   \return The number of bytes written to the stream.  In the base
   implementation of AFileStream, this value is always zero (0).
*}
function AFileStream.write(const count: TStreamIOSize;
  const source): TStreamIOSize;

begin
  // Does nothing here; this is for descendant classes to implement
  result := 0;
end;

{$I-}
{* Closes the file associated with the stream.  This routine calls
   System::close().  If the AFileStream instance does not represent a valid
   file, then this routine does nothing.
*}
procedure AFileStream.close;

begin
  if myHandleIsValid then
  begin
    System.close(myFile);
  end;
end;
{$I+}

/// Returns the name of the file being streamed, if available
function AFileStream.name: string;

begin
  if (myHandleIsValid) and (myFileName <> '') then
    result := myFileName

  else
    result := inherited name;
end;

// IsPrintable implementation --------------------------------------------------
{* Returns a string representation of the class instance.  This routine first
   attempts to determine the name of the file being streamed through a call
   to AFileStream.fileName; failing that, it returns the display name of the
   class through a call to AnObject.displayName.

   \return A string representation of the class instance, as described above.
*}
function AFileStream.toString: string;

begin
  result := Self.name;
end;

{* Prints a string representation of the class to \p Dest.  The string
   representation of the class is that produced by AFileStream.toString.

   \return The total number of bytes written to \p Dest.  This value will be
   zero (0) if \p Dest is #nil.
*}
function AFileStream.printTo(const Dest: AStream;
  prefix: AnsiString = '';
  suffix: AnsiString = ''): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
  begin
    result := Dest.printEach([prefix, Self.toString, suffix]);
  end;
end;

// Properties ------------------------------------------------------------------
{* Retrieves the current position within the stream.  This routine makes use
   of the filePos function in the System unit of Free Pascal.

   \return A value indicating the current position within the stream.  This
   value is simply the number of bytes from the beginning of the file to
   the present position within the file.  The value returned will be zero (0)
   if the instance of AFileStream does not represent a valid file.
*}
function AFileStream.position: TStreamOffset;

begin
  result := 0;
  if myHandleIsValid then
    result := System.filePos(myFile);
end;

{* Sets the current position within the stream.

   If \p newPosition is a value of zero (0) or greater, it represents the
   number of bytes from the beginning of the stream to the desired position
   within the stream.  The beginning of the stream is always at offset zero (0).

   If \p newPosition is a value that is less than zero, then the value
   represents the number of bytes from the END of the stream to the desired
   position.  The end of the stream is always at offset -1.

   If \p newPosition is greater than AFileStream::length() or the AFileStream
   instance does not represent a valid file, this routine does nothing.

   \return The previous position within the stream.
*}
function AFileStream.setPosition(newPosition: TStreamOffset): TStreamOffset;

begin
  result := -1;

  if (myHandleIsValid) and (newPosition <= Self.length) then
  begin
    // Store what will be the previous position
    result := System.filePos(myFile);

    // If the new position is less than zero, it represents a position
    // relative to the END of the file
    if newPosition < 0 then
      newPosition := newPosition + Self.length + 1;

    if newPosition < 0 then
      newPosition := 0;

    // Seek the new position in the stream
    System.seek(myFile, newPosition);
  end;
end;

{* Retrieves the size of the file stream, in bytes.  This routine calls
   System.fileSize.

   \return A value indicating the size of the file stream, in bytes.  This
   value will be zero (0) if the file was newly-created or if the AFileStream
   instance does not represent a valid file.
*}
function AFileStream.length: TStreamLength;

begin
  result := 0;
  if myHandleIsValid then
    result := fileSize(myFile);
end;

{* Determines whether or not the end of the stream has been reached.  This
   routine calls System.eof.

   \return #true if the end of the stream has been reached; #false if not.
   This routine will also return #false if the AFileStream instance does not
   represent a valid file.
*}
function AFileStream.hasEnded: boolean;

begin
  result := true;
  if myHandleIsValid then
    result := eof(myFile);
end;

/// Retrieve the handle of the file being streamed
function AFileStream.handle: pointer;

begin
  result := @myFile;
end;

/// Retrieve the name of the file being streamed
function AFileStream.fileName: string;

begin
  result := myFileName;
end;

/// Determine whether the handle for the underlying file is valid
function AFileStream.handleIsValid: boolean;

begin
  result := myHandleIsValid;
end;

// AFileInputStream ************************************************************
{$I-}
{* Constructs a new file input stream for the file specified by \p thisName.
   The file must already exist.  If \p thisName is an empty string or an
   error occurs while attempting to open the specified file, an exception is
   thrown.

   \return A new instance of AFileInputStream on success; #nil on failure.
   The call will fail and an exception will be thrown if \p thisName is invalid
   (an empty string) or the file it names cannot be opened.
*}
constructor AFileInputStream.fromFileNamed(const thisName: string);

var
  // Stores the outcome of an I/O operation
  error: longword;
  // Stores the old file mode, since FPC now has a bug
  oldFileMode: byte;

begin
  inherited new;
  if thisName = '' then
    raise AFileStreamOpenError.because(fserrInvalidFileName, []);

  myFileName := thisName;
  System.assign(myFile, myFileName);

  { Work around FPC bug that assumes '2' is a valid file mode on all platforms.
    Get access-denied error on a read-only file with reset (which should only
    open read-only) on Linux.
  }
  oldFileMode := System.fileMode;
  System.fileMode := 0;

  reset(myFile, sizeof(byte));
  System.fileMode := oldFileMode;

  error := ioResult;
  if error = 0 then
    myHandleIsValid := true

  else
    raise AFileStreamOpenError.because(fserrOpenRead, [
      thisName, ioMessage(error)
    ]);
end;

{* Reads \p count bytes from the stream and stores the data in \p dest.  This
   routine uses System.blockRead to read from the file.

   \return The total number of bytes read from the file.  This value will be
   zero (0) if the end of the file was reached before this call was made, or
   if \p count is zero.  If an error occurs while reading from the file,
   an exception will be thrown and this routine will not return.
*}
function AFileInputStream.read(const count: TStreamIOSize;
  var dest): TStreamIOSize;

var
  // Refers to an I/O error that can occur while reading from a stream
  error: longword;
  // Stores the number of bytes actually read
  amountRead: TStreamIOSize;

begin
  result := 0;
  if (Self.hasEnded = false) and (count > 0) then
  begin
    amountRead := 0;
    blockRead(myFile, dest, count, amountRead);
    result := amountRead;
    error := ioResult;

    if error <> 0 then
      raise AFileStreamReadError.because(fserrReading, [
        myFileName, ioMessage(error)
      ]);
  end;
end;
{$I+}

// AFileOutputStream ***********************************************************
{$I-}
{* Constructs a new file output stream for the file specified by \p thisName.
   If \p overwrite is #true and the file already exists, it will be
   overwritten by this call.  If \p overwrite is #false and the file already
   exists, this routine will open the file for additional data.  If the file
   does not exist, it will be created.

   \return A new instance of AFileOutputStream on success; #nil on failure.
   The call will fail if \p thisName is invalid (an empty string) and an
   exception will be thrown.  An exception will also be thrown if an error
   occurs while attempting to open or create the named file.
*}
constructor AFileOutputStream.toFileNamed(const thisName: string;
  const overwrite: boolean = false);

var
  // Refers to an I/O error that can occur while attempting to open a file
  error: longword;

begin
  inherited new;
  if thisName = '' then
    raise AFileStreamOpenError.because(fserrInvalidFileName, []);

  myFileName := thisName;
  System.assign(myFile, myFileName);
  if (fileExists(myFileName)) and (overwrite = false) then
    reset(myFile, sizeof(byte))
  else
    rewrite(myFile, sizeof(byte));
  error := ioResult;
  if error = 0 then
    myHandleIsValid := true
  else
    raise AFileStreamOpenError.because(fserrOpenWrite, [
      thisName, ioMessage(error)
    ]);
end;

{* Writes \p count bytes from \p source to the stream.

  \return The total number of bytes written to the stream.  This value will
  be zero (0) if \p count is zero.  If an error occurs while writing to the
  file, an exception will be thrown and the routine will not return.
*}
function AFileOutputStream.write(const count: TStreamIOSize;
  const source): TStreamIOSize;

var
  // Refers to an I/O error that can occur while writing to a file
  error: longword;
  // Stores the number of bytes written
  amountWritten: TStreamIOSize;

begin
  result := 0;
  if count > 0 then
  begin
    amountWritten := 0;
    blockWrite(myFile, source, count, amountWritten);
    result := amountWritten;
    error := ioResult;

    if error <> 0 then
      raise AFileStreamWriteError.because(fserrWriting, [
        myFileName, ioMessage(error)
      ]);
  end;
end;
{$I+}

{$ENDIF}

