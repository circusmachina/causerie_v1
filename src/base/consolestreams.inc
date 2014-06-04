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
{ This class allows access to @code(stdout) as though it was a file stream.  The
  class does not support reading or seeking and will fail silently if these
  operations are attempted.  The primary reason for this class to exist is to
  allow console output to be used in conjunction with any class that supports
  writing to an arbitrary stream.
}
type AStandardOutputStream = class(AStream)
  protected
    { Stores the current position within the stream }
    myPosition: TStreamOffset;

  public
    { Initializer }
    function init: boolean; override;

    { Read @code(count) bytes from the stream and store them in @code(dest).

      This method fails silently for this class and will always return zero (0).
    }
    function read(const count: TStreamIOSize; var dest): TStreamIOSize;
      override;

    { Write @code(count) bytes from @code(source) to the stream.
      See @link(AStream.write) for more information on this method.

      @return(The total number of bytes written to the stream.)
    }
    function write(const count: TStreamIOSize; const source): TStreamIOSize;
      override;

    // Properties --------------------------------------------------------------
    { Retrieve the current position within the stream.  This value is simply
      the number of bytes that have been written to the stream since it was
      created; for @link(AStandardOutputStream), therefore, it is always the
      same value as @link(AStandardOutputStream.length).
    }
    function position: TStreamOffset; override;

    { Set the current position within the stream.

      This method fails silently for this class and will simply return the
      same value as @link(AStandardOutputStream.position).
    }
    function setPosition(newPosition: TStreamOffset): TStreamOffset; override;

    { Retrieve the size of the stream, in bytes.  This value is simply the
      number of bytes that have been written to the stream since it was
      created.
    }
    function length: TStreamLength; override;
end;

{ This class allows access to @code(stdin) as though it was a file stream.  The
  class does not support writing or seeking and will fail silently if these
  operations are attempted.  The primary reason for this class to exist is to
  allow console input to be used by any class that supports reading from an
  arbitrary stream.
}
type AStandardInputStream = class(AStream)
  protected
    { Stores the current position within the stream }
    myPosition: TStreamOffset;

  public
    { Initializer }
    function init: boolean; override;

    { Read @code(count) bytes from the stream and store them in @code(dest).
      See @link(AStream.read) for more information on this method.

      @return(The total number of bytes read from the stream.)
    }
    function read(const count: TStreamIOSize; var dest): TStreamIOSize;
      override;

    { Write @code(count) bytes from @code(source) to the stream.

      This method fails silently for this class and will always return zero (0).
    }
    function write(const count: longword; const source): TStreamIOSize;
      override;

    // Properties --------------------------------------------------------------
    { Retrieve the current position within the stream.  This value is simply
      the number of bytes that have been read from the stream since it was
      created; for @link(AStandardInputStream), therefore, it is always the
      same value as @link(AStandardInputStream.length).
    }
    function position: TStreamOffset; override;

    { Set the current position within the stream.

      This method fails silently for this class and will simply return the
      same value as @link(AStandardInputStream.position).
    }
    function setPosition(newPosition: TStreamOffset): TStreamOffset; override;

    { Retrieve the size of the stream, in bytes.  This value is simply the
      number of bytes that have been read from the stream since it was
      created.
    }
    function length: TStreamLength; override;
end;

{ This class allows access to @code(stderr) as though it was a file stream.
  The class does not support reading or seeking and will fail silently if these
  operations are attempted.  The primary reason for this class to exist is to
  allow console output to be used by any class that supports writing to an
  arbitrary stream.
}
type AStandardErrorStream = class(AStandardOutputStream)
  public
    { Write @code(count) bytes from @code(source) to the stream.
      See @link(AStream.write) for more information on this method.

      @return(The total number of bytes written to the stream.)
    }
    function write(const count: TStreamIOSize; const source): TStreamIOSize;
      override;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// AStandardOutputStream *******************************************************
{* Initializes a new instance of AStandardOutputStream.  This routine sets
   AStandardOutputStream.position to zero (0) and then calls the inherited
   routine.  In the default implementation, therefore, it always returns
   #true.

   \return #true if the new instance was initialized successfully; #false if
   not.  In the default implementation, this routine will always return #true.
*}
function AStandardOutputStream.init: boolean;

begin
  myPosition := 0;
  result := inherited init;
end;

{* \p stdout is a write-only, sequential file.  As such, it does not support
   reading and so attempts to read from the stream will fail.  This routine
   is included because AStream, as the abstract ancestor class, requires it to
   be implemented.  It does nothing.

   \return The total number of bytes read from the stream.  For \p stdout,
   this value will always be zero (0).
*}
function AStandardOutputStream.read(const count: TStreamIOSize;
  var dest): TStreamIOSize;

begin
  // The stream cannot be read
  result := 0;
end;

{* Writes \p count bytes from \p source to the stream.  \p source should
   be a string, since \p stdout is designed for text output and will not
   correctly represent binary data that is sent to it.

   \return The total number of bytes written to the stream.  If \p count is
   zero (0), this value will also be zero.
*}
function AStandardOutputStream.write(const count: TStreamIOSize;
  const source): TStreamIOSize;

var
  // Stores the new string
  newString: string;
  // Refers to a source character
  psource: pchar;

begin
  result := 0;
  if count > 0 then
  begin
    newString := '';
    psource := pchar(@source);
    // Get the new string
    system.setString(newString, psource, count);
    result := system.length(newString);
    inc(myPosition, result);
    system.write(stdout, newString);
  end;
end;

// Properties ------------------------------------------------------------------
{* Retrieves the current position within the stream.  For AStandardOutputStream,
   this value is simply the number of bytes that have been written to the stream
   over the lifetime of the class instance.

   \return The current position within the stream, which is the number of bytes
   that have been written to the stream since it was opened.
*}
function AStandardOutputStream.position: TStreamOffset;

begin
  result := myPosition;
end;

{* \p stdout is a write-only, sequential file.  As such, it does not support
   random access and so attempts to set the current position within the stream
   will fail.  This routine is included because AStream, as the abstract
   ancestor class, requires it to be implemented.  It does nothing.
*}
function AStandardOutputStream.setPosition(newPosition: TStreamOffset): TStreamOffset;

begin
  result := myPosition;
end;

{* Retrieves the size of the stream.  This value is merely the number of bytes
   that have been written to the stream since it was opened.

   \return The size of the stream, in bytes.
*}
function AStandardOutputStream.length: TStreamLength;

begin
  result := myPosition;
end;

// AStandardInputStream ********************************************************
{* Initializes a new instance of AStandardInputStream.  This routine sets
   AStandardInputStream.position to zero (0) and then calls the inherited
   routine.  This means that, in the default implementation, it will always
   return #true.

   \return #true if the new instance was initialized successfully; #false if
   not.  In the default implementation, this routine always returns #true.
*}
function AStandardInputStream.init: boolean;

begin
  myPosition := 0;
  result := inherited init;
end;

{* Reads \p count bytes from the stream and stores them in \p dest.  This
   routine calls System.read; as a result, if \p stdin has not been redirected
   away from the keyboard, it will wait for the specified number of characters
   to be entered before returning.

   \return The total number of bytes read from the stream.  If \p count is
   zero (0), this value will also be zero.
*}
function AStandardInputStream.read(const count: TStreamIOSize;
  var dest): TStreamIOSize;

var
  // Refers to the destination character
  pdest: pchar;
  // A loop counter
  i: TStreamIOSize;

begin
  result := 0;
  if count > 0 then
  begin
    pdest := pchar(@dest);
    for i := 1 to count do
    begin
      system.read(input, pdest^);
      inc(pdest);
      inc(result);
      inc(myPosition);
    end;
  end;
end;

{* \p stdin is a read-only, sequential file.  As such, it does not support
   writing and so attempts to write to the stream will fail.  This routine is
   included because AStream, as the abstract ancestor class, requires it to be
   implemented.  It does nothing.

   \return The total number of bytes written to the stream.  For \p stdin, this
   value will always be zero (0).
*}
function AStandardInputStream.write(const count: TStreamIOSize;
  const source): TStreamIOSize;

begin
  // The stream cannot be written
  result := 0;
end;

// Properties ------------------------------------------------------------------
{* Retrieves the current position within the stream.  This value is merely the
   number of bytes that have been read by the AStandardInputStream instance
   since its initialization.

   \return The current position within the stream, which is merely the number of
   bytes that have been read from the stream.
*}
function AStandardInputStream.position: TStreamOffset;

begin
  result := myPosition;
end;

{* \p stdin is a read-only, sequential file.  As such, it does not support
   random access and so attempts to set the current position within the stream
   will fail.  This routine is included because AStream, as the abstract
   ancestor class, requires it to be implemented.  It does nothing.
*}
function AStandardInputStream.setPosition(newPosition: TStreamOffset): TStreamOffset;

begin
  result := myPosition;
end;

{* Retrieves the size of the stream, in bytes.  This value is merely the total
   number of bytes that have been read from the stream since it was initialized.

   \return The size of the stream, in bytes.
*}
function AStandardInputStream.length: TStreamLength;

begin
  result := myPosition;
end;

// AStandardErrorStream ********************************************************
{* Writes \p count bytes from \p source to the stream.  \p source should
   be a string, since \p stderr is designed for text output and will not
   correctly represent binary data that is sent to it.

   \return The total number of bytes written to the stream.  If \p count is
   zero (0), this value will also be zero.
*}
function AStandardErrorStream.write(const count: TStreamIOSize;
  const source): TStreamIOSize;

var
  // Stores the new string
  newString: string;
  // Refers to a source character
  psource: pchar;

begin
  result := 0;
  if count > 0 then
  begin
    newString := '';
    psource := pchar(@source);
    // Get the new string
    system.setString(newString, psource, count);
    result := system.length(newString);
    inc(myPosition, result);
    system.write(stderr, newString);
  end;
end;
{$ENDIF}

