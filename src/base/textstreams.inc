{ bufferedstreams.inc - Classes to manipulate stream data as text.

  This file is part of classwork.pp, a library of basic and useful classes.

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
{ This class serves as the basis for a class which wraps around other
  instances of @link(AStream) and provides the ability to treat the data
  read from the stream as text.  In particular, this class supports reading from
  ASCII- or UTF-8-encoded streams, as well as providing methods for reading
  and returning an entire line of text.
}
type ATextInputStream = class(AStreamWrapper)
  public
    { Read @code(count) bytes from the buffer and store the data in @code(dest).

      This method simply passes the read call through to
      @link(AStreamWrapper.Target).  It does not attempt to interpret the
      data read.  To retrieve one or more characters, call
      @link(ATextInputStream.readCharacters).  To retrieve a line of text,
      call @link(ATextInputStream.readLine).

      @return(The total number of bytes read from the stream.  This value may
      be less than @code(count) if the end of the stream was encountered while
      reading.)
    }
    function read(const count: TStreamIOSize; var dest): TStreamIOSize;
      override;

    { Read @code(count) characters from the stream and store the data in
      @code(dest).

      @code(dest) is set to an empty string by this routine before it begins to
      read.

      This method reads from the underlying target stream and stores the
      desired number of characters in @code(dest).  As it reads from the stream,
      the method attempts to determine whether or not the character is encoded
      using ASCII or UTF-8 and adjusts accordingly.

      @return(The total number of @italic(characters) read from the stream.
      This value may be less than @code(count) if the end of the stream was
      encountered while reading.)
    }
    function readCharacters(count: TStreamIOSize;
      var dest: AnsiString): TStreamIOSize; virtual;

    { Read characters from the stream until the end of a line (or the end of the
      stream itself) is encountered.

      @code(dest) is set to an empty string by this routine before it begins to
      read.

      This method reads from the underlying target stream and stores characters
      in @code(dest) until @code(lineTerminator) is reached.  The line
      terminator characters will not be added to @code(dest), but the next call
      to @link(ATextInputStream.readLine) will begin at the start of the next
      line in the underlying stream, unless the end of the stream is reached
      first.

      @return(The total number of @italic(characters) read from the stream.)
    }
    function readLine(var dest: AnsiString;
      const lineTerminator: string = System.lineEnding): TStreamIOSize;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// ATextInputStream ************************************************************
// Read from the underlying stream
function ATextInputStream.read(const count: TStreamIOSize;
  var dest): TStreamIOSize;

begin
  result := 0;

  // Pass the call onto the target
  if MyTarget <> nil then
    result := MyTarget.read(count, dest);
end;

// Read characters from the underlying stream
function ATextInputStream.readCharacters(count: TStreamIOSize;
  var dest: AnsiString): TStreamIOSize;

var
  // Stores a character read from the stream
  ch: string;
  // Stores the length of a character read from the stream
  charLength: integer;

begin
  result := 0;
  dest := '';

  if MyTarget = nil then
    exit;

  while count > 0 do
  begin
    ch := '';
    // A UTF-8 character will have, at most, 4 bytes
    System.setLength(ch, sizeof(longword));

    // Check to see if we've reached the end of the target stream
    if MyTarget.hasEnded then
      break;

    // Read the character; at worst, we get 4 ASCII characters
    MyTarget.read(sizeof(longword), ch[1]);

    // Get the length of the first character in the string
    charLength := Charstring.characterLength(ch);
    // Copy that number of bytes to our destination
    dest := dest + System.copy(ch, 1, charLength);
    inc(result);

    // Rewind the stream by the difference
    MyTarget.rewindBy(sizeof(longword) - charLength);
    dec(count);
  end;
end;

// Read a line from the underlying stream
function ATextInputStream.readLine(var dest: AnsiString;
  const lineTerminator: string = System.lineEnding): TStreamIOSize;

var
  // Stores a sequence of characters read from the stream
  ch: string;

begin
  result := 0;
  dest := '';

  if MyTarget = nil then
    exit;

  while not MyTarget.hasEnded do
  begin
    ch := '';
    System.setLength(ch, System.length(lineTerminator));

    // Read some data
    MyTarget.read(System.length(ch), ch[1]);
    // Check to see if we've reached the line terminator
    if ch = lineTerminator then
      break;

    dest := dest + ch;
    inc(result);
  end;
end;
{$ENDIF}

