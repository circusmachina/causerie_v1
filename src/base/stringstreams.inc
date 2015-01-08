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
{ This class allows an @code(AnsiString) to be read from and written to as
  though it was a stream.  It is useful in those instances when buffering data
  in memory is desired, particularly because the class is capable of streaming
  its contents to and from another stream.

  The stream supports reading, writing, and seeking.  Read and write operations
  always occur at the location specified by @link(AStringStream.position); this
  value can be changed by first calling @link(AStringStream.setPosition).

  Regardless of how the parent unit is compiled (with or without
  @code(AnsiString)s as the default string type), this class will always operate
  on an @code(AnsiString).
}
type AStringStream = class(AStream, CanStream, CanPrint)
  protected
    { Stores the name of the stream. }
    myName: string;
    { Stores the string data used by the stream. }
    myString: AnsiString;
    { Stores the current position within the stream. }
    myPosition: TStreamOffset;

  public
    { Construct a new stream instance.

      If @code(str) is provided, its value is used as the initial content of the
      stream.  @code(str) is not written to the stream; it is simply assigned
      as the content, which provides a quick way to buffer a string and then
      iterate over its contents.
    }
    constructor new(const str: AnsiString = ''); reintroduce; virtual;

    { Initializer }
    function init: boolean; override;

    { Read @code(count) bytes from the stream and store them in @code(dest).
      See @link(AStream.read) for more information on this method.

      @return(The total number of bytes read from the stream.)
    }
    function read(const count: TStreamIOSize; var dest): TStreamIOSize;
      override;

    { Write @code(count) bytes from @code(source) to the stream.
      See @link(AStream.write) for more information on this method.

      Note that no checks are made on the data to ensure that what is written
      will display well on text-based devices.

      @return(The total number of bytes written to the stream.)
    }
    function write(const count: TStreamIOSize; const source): TStreamIOSize;
      override;

    { Clear the stream.  The contents of the underlying string are discarded,
      and @link(AStringStream.position) and @link(AStringStream.length) are
      both reset to zero (0).  Subsequent attempts to read from the stream
      without first writing content to it will fail silently, and
      @link(AStream.hasEnded) will return @true.
    }
    procedure clear; virtual;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(AnObject.shallowCopyFrom): it calls that method, then checks to
      see whether @code(Other) is an instance of @link(AStringStream).  If so,
      it copies the value of
      @unorderedList(
        @item(@link(AStringStream.text) and)
        @item(@link(AStringStream.position))
      )
      from @code(Other) to @code(Self), overwriting any values in @code(Self).
      The end result in this case is a string stream that is an exact copy of
      @code(Other).
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // CanStream implementation ------------------------------------------------
    { Determine the total amount of data, in bytes, that will be read or
      written when either @link(AStringStream.readFrom) or
      @link(AStringStream.writeTo) is called.
    }
    function streamingLength: TStreamIOSize; virtual;

    { Writes the content of the stream to @code(Dest).  See
      @link(CanStream.writeTo) for more information on this method.

      This method calls @link(AStream.writeString) on @code(Dest), which means
      the length and checksum of the underlying string are also written.

      @return(The total number of bytes written to @code(Dest), which will
      include the length and checksum information for the underlying string.)
    }
    function writeTo(const Dest: AStream): TStreamIOSize; virtual;

    { Reads the contents of the stream from @code(Source).  See
      @link(CanStream.readFrom) for more information on this method.

      This method calls @link(AStream.readString) on @code(Source), which means
      that it will first read the length and checksum of the underlying string.

      @return(The total number of bytes read from @code(Source), which will
      include the length and checksum information for the underlying string.)
    }
    function readFrom(const Source: AStream): TStreamIOSize; virtual;

    // IsPrintable implementation ----------------------------------------------
    { Constructs and returns a string representation of the stream, suitable
      for output to a text-based device.  See @link(CanPrint.toString) for more
      information on this method.

      This method simply returns the value of @link(AStringStream.text), which
      is the underlying content of the stream.
    }
    function toString: string; override;

    { Print a string representation of the stream to @code(Dest).  See
      @link(CanPrint.printTo) for more information on this method.

      This method simply prints the value returned by a call to
      @link(AStringStream.toString) to @code(Dest).

      @return(The total number of bytes printed to @code(Dest).)
    }
    function printTo(const Dest: ATextOutputStream;
      prefix: AnsiString = '';
      suffix: AnsiString = ''): TStreamIOSize; virtual;

  // Properties ----------------------------------------------------------------
    { Retrieve the name of the stream.

      This method overrides the behavior inherited from
      @link(AnObject.name): instead of return the class name of the stream, it
      returns the name set by a previous call to
      @link(AStringStream.setName).  If no such call has been made, an empty
      string is returned.
    }
    function name: string; override;

    { Set the name of the stream.

      This method allows the caller to set the name that is reported when
      @link(AStringStream.name) is called.  This behavior is intended to allow
      a descriptive name to be given to instances of @link(AStringStream) for
      display in debug and diagnostic messages.

      @return(The previous value of @link(AStringStream.name).)
    }
    function setName(const newName: string): string; virtual;

    { Retrieve the current position within the stream.  This is the location at
      which the next read or write operation will occur.

      See @link(AStream.position) for more information on this method.
    }
    function position: TStreamOffset; override;

    { Set the current position within the stream.  This is the location at
      which the next read or write operation will occur.

      See @link(AStream.setPosition) for more information on this method.

      @return(The previous position within the stream.)
    }
    function setPosition(newPosition: TStreamOffset): TStreamOffset; override;

    { Retrieve the current size of the stream, in bytes.  See
      @link(AStream.length) for more information on this method.

      This method returns the length of the underlying string, as determined
      by a call to @code(System.length).
    }
    function length: TStreamLength; override;

    { Retrieve the content of the stream.  The string returned by this routine
      will contain all values written to the stream, or the content of the
      string specified by a call to @link(AStringStream.setText).
    }
    function text: AnsiString; virtual;

    { Set the content of the stream.  The current content of the stream will be
      discarded in favor of the value specified by @code(newText).
      @link(AStream.position) will be reset to zero (0), and
      @link(AStream.length) will return the length of @code(newText).

      @return(The old content of the stream.)
    }
    function setText(const newText: AnsiString): AnsiString; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
/// AStringStream **************************************************************
/// Constructor
constructor AStringStream.new(const str: AnsiString = '');

begin
  inherited new;

  myString := str;
end;

{* Initializes a new instance of AStringStream.  This routine clears the string
   used by the stream and sets AStringStream.position to zero (0) before
   calling the inherited routine.  In the default implementation, therefore,
   this routine will always return #true.

   \return #true if the new instance was initialized successfully; #false if
   not.  In the default implementation, this routine always returns #true.
*}
function AStringStream.init: boolean;

begin
  myName := '';
  myString := '';
  myPosition := 0;
  result := inherited init;
end;

{* Reads \p count bytes from the stream and stores the data in \p dest.

   \return The total number of bytes read from the stream.  This value may be
   less than \p count if the end of the string was reached before \p count
   bytes were read from the string.  It will be zero (0) if the end of the
   string was reached before the routine was called, or if \p count itself is
   zero.
*}
function AStringStream.read(const count: TStreamIOSize;
  var dest): TStreamIOSize;

var
  // Stores the starting and ending positions within the string
  startPos, endPos: TStreamOffset;
  // Refers to the destination character
  pdest: pchar;
  // A loop counter
  i: TStreamIOSize;

begin
  result := 0;

  if (count > 0) and (Self.hasEnded = false) then
  begin
    // Calculate the start and end positions for the read from the string
    endPos := myPosition + count;
    startPos := myPosition + 1;

    if endPos > System.length(myString) then
      endPos := System.length(myString);

    pdest := pchar(@dest);

    for i := startPos to endPos do
    begin
      pdest^ := myString[i];
      inc(pdest);
      inc(result);
      inc(myPosition);
    end;
  end;
end;

{* Writes \p count bytes from \p source to the stream.

   \return The total number of bytes written to the stream.  This value will
   be zero (0) if \p count is zero.
*}
function AStringStream.write(const count: TStreamIOSize;
  const source): TStreamIOSize;

var
  // Stores the start position within the string
  startPos: TStreamOffset;
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
    System.setString(newString, psource, count);
    result := System.length(newString);
    startPos := myPosition + 1;

    // If we are writing from the beginning of the string...
    if startPos = 1 then
    begin
      myString := newString;
      myPosition := result - 1;
    end

    // If we are writing from the end of the string...
    else if startPos = system.length(myString) then
    begin
      myString := myString + newString;
      inc(myPosition, result);
    end

    // If we are starting from somewhere in the middle of the string
    else begin
      myString := copy(myString, 1, myPosition) + newString;
      myPosition := System.length(myString) - 1;
    end;
  end;
end;

/// Clears the stream
procedure AStringStream.clear;

begin
  myString := '';
  myPosition := 0;
end;

// Construct a shallow copy of the given object
function AStringStream.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other item is compatible with us
  if not(Other is AStringStream) then
    result := false;

  if result then
  begin
    myString := AStringStream(Other).text;
    myPosition := AStringStream(Other).position;
  end;
end;

// CanStream implementation ----------------------------------------------------
// Determine the size, in bytes, of the data to be streamed
function AStringStream.streamingLength: TStreamIOSize;

begin
  // Total value streamed is stream length + length specifier + length checksum
  result := System.length(myString) + (sizeof(TStreamIOSize) * 2);
end;

{* Writes the contents of the stream to \p Dest.  This routine calls
   AStream.writeString().

   \return The total number of bytes written to \p Dest.  This value will be
   zero (0) if \p Dest is #nil.
*}
function AStringStream.writeTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
    result := Dest.writeString(myString);
end;

{* Reads the contents of the stream from \p Source.  This routine calls
   AStream.readString().

   \return The total number of bytes read from \p Source.  This value will be
   zero (0) if \p Source is #nil.
*}
function AStringStream.readFrom(const Source: AStream): TStreamIOSize;

begin
  result := 0;

  if Source <> nil then
    result := Source.readString(myString);
end;

// CanPrint implementation -----------------------------------------------------
{* Returns a string representation of the stream.  This is merely a reference to
   the string being manipulated by the stream.

   \return A string representation of the stream, as described above.
*}
function AStringStream.toString: AnsiString;

begin
  result := myString;
end;

{* Prints a string representation of the stream to \p Dest.  This routine
   calls AStream.printEach.

   \return The total number of bytes written to \p Dest, which will be zero (0)
   if \p Dest is #nil.
*}
function AStringStream.printTo(const Dest: ATextOutputStream;
  prefix: AnsiString = '';
  suffix: AnsiString = ''): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
    result := Dest.printEach([prefix, Self.toString, suffix]);
end;

// Properties ------------------------------------------------------------------
// Retrieve the name of the stream
function AStringStream.name: string;

begin
  result := myName;
end;

// Set the name of the stream
function AStringStream.setName(const newName: string): string;

begin
  result := myName;
  myName := newName;
end;

{* Retrieves the current position within the stream.

   \return The current position within the stream, which is the number of bytes
   between the beginning of the string and its present position.
*}
function AStringStream.position: TStreamOffset;

begin
  result := myPosition;
end;

{* Sets the current position within the stream.  Subsequent reads or write
   operations on the string will occur at \p newPosition.

   If \p newPosition is a value of zero (0) or greater, then it represents
   the number of bytes between the beginning of the stream and the desired
   position therein.  The beginning of the stream is always at offset zero (0).

   If \p newPosition is a value that is less than zero, then it represents the
   number of bytes between the END of the stream and the desired position
   therein.  The end of the stream is always at offset -1.

   If \p newPosition is greater than or equal to the length of the stream, this
   routine does nothing.

   \return The previous position in the stream.
*}
function AStringStream.setPosition(newPosition: TStreamOffset): TStreamOffset;

begin
  result := myPosition;

  if newPosition < 0 then
    newPosition := newPosition + System.length(myString) + 1;

  if newPosition < 0 then
    newPosition := 0;

  if newPosition < System.length(myString) then
    myPosition := newPosition;
end;

{* Retrieves the size of the stream.  This is the length of the string being
   manipulated.  This routine uses System::length() to determine the length of
   the string.

   \return The size of the stream, in bytes.
*}
function AStringStream.length: TStreamLength;

begin
  result := System.length(myString);
end;

/// Retrieves the text of the stream
function AStringStream.text: AnsiString;

begin
  result := myString;
end;

/// Sets the text used by the stream
function AStringStream.setText(const newText: AnsiString): AnsiString;

begin
  result := myString;

  myPosition := 0;
  myString := newText;
end;
{$ENDIF}

