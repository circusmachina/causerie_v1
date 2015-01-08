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

{ This class serves as the basis for a class which wraps around other
  instances of @link(AStream) and provides the ability to treat the data
  to be written to the stream as text.  In particular, this class supports
  printing ASCII- or UTF-8-encoded streams, as well as Unicode strings and
  @code(WideStrings).
}
type ATextOutputStream = class(AStreamWrapper)
  public
    { Write @code(count) bytes from @code(source) to the stream.

      This method simply passes the read call through to
      @link(AStreamWrapper.Target).  It does not attempt to interpret the
      data written.  To print one or more strings, call
      @link(ATextOutputStream.print) or @link(ATextOutputStream.printEach).

      @return(The total number of bytes written to the stream.  This value may
      be less than @code(count) if the end of the stream was encountered while
      writing.)
    }
    function write(const count: TStreamIOSize; const source): TStreamIOSize;
      override;

    { Print the specificed string to the stream.

      This method will print the contents of @code(str) to the stream.  It is
      primarily designed to be used with text-based output streams, such as
      @link(AStandardOutputStream), since it does not record the length of the
      string so that it can be read later.  To write a string, including its
      length, to the stream, use @link(AStream.writeString) instead.

      @return(The total number of bytes printed to the stream.  This value will
      be zero if @code(str) is an empty string.)
    }
    function print(const str: AnsiString): TStreamIOSize; virtual;

    { Print each value in @code(vals) to the stream.

      This method is defined and implemented as a convenience function; it
      writes a string representation of each variable provided in @code(vals) to
      the stream.

      It is possible to pass instances of objects to this routine, but they
      must be members of a class that implements @link(CanPrint).  This routine
      will call the @code(printTo) method for all such instances passed.  All
      other object instances passed to this routine are ignored.

      If a class reference is passed to this routine, it will output a string
      containing the name of the class.

      @return(The total number of bytes printed to the stream.  This value may
      be less than expected if certain items in @code(vals) were skipped.)
    }
    function printEach(const vals: array of const): TStreamIOSize; virtual;

    { Print the selected string to the stream, breaking into lines and then
      wrapping and justifying those lines as necessary.

      @code(str) is broken into one or more lines that are no more than
      @code(thisLineLength) characters in length.  If @code(thisLineLength) is
      zero (0), then this method uses the value of
      @link(ATextOutputStream.lineLength) instead.

      @code(str) is broken into lines only where the characters specified by
      @code(breakCharacters) are encountered within the string.  The default
      characters in @link(tosWrapDefaultBreakCharacters) can be used if the
      caller does not wish to specify other characters.  In any case,
      @code(breakCharacters) should contain ASCII or UTF-8 encoded characters.
      Only the first such character from each element in @code(breakCharacters)
      is used; if the strings in @code(breakCharacters) contain more than one
      character, the extraneous characters are ignored.

      The first line that is printed is padded on the left by a repeating
      sequence of the characters specified by @code(padCharacter), which should
      contain an ASCII or UTF-8 encoded character.  Only the first such
      character from @code(padCharacter) is used; if it contains others, they
      are ignored.  The number of times @code(padCharacter) is used for the
      first line is determined by @code(firstLineStartColumn); if that value is
      zero, then the first line is not padded.

      The remaining lines that are printed are padded on the left by the same
      repeating sequence of characters specified by @code(padCharacter).  The
      number of times @code(padCharacter) is used for each remaining line is
      determined by @code(remainingLinesStartColumn); if that value is zero,
      the remaining lines are not padded.

      If @code(padCharacter) is an empty string, the value specified by
      @link(tosWrapDefaultPadCharacter) is used.

      For more information on the various parameters accepted by this routine,
      see @link(AStringList.wrapString).

      @return(The total number of bytes printed to the stream.  If @code(str) is
      an empty string, this method does nothing and returns zero (0).)
    }
    function printWrapped(const str: AnsiString;
      const breakCharacters: array of string;
      thisLineLength: longword = 0;
      const firstLineStartColumn: longword = 0;
      const remainingLinesStartColumn: longword = 0;
      padCharacter: string = '';
      const lineBreakCharacter: string = System.lineEnding): TStreamIOSize;

    // Properties --------------------------------------------------------------
    { Determine the maximum length of a printed line, in characters.

      The value returned by this method indicates the maximum number of
      characters that may be printed on a single line before the line is
      forcibly wrapped by the underlying device.

      The base implementation of this method simply returns the value of
      @link(tosDefaultLineLength), but descendant classes may override this
      behavior to return a different value.

      This value is used by @link(ATextOutputStream.printWrapped) to wrap text
      so that it fits within the boundaries of the target stream.
    }
    function lineLength: longword; virtual;
end;

const
  { This constant controls the maximum length of a line, in characters, that is
    wrapped by @link(ATextOutputStream.printWrapped).
  }
  tosDefaultLineLength = 79;

  { This array contains the default characters at which a line wrapped by
    @link(ATextOutputStream.printWrapped) is broken.
  }
  tosWrapDefaultBreakCharacters: array[0..2] of string = (
    ' ', '-', System.lineEnding);
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This constant specifies the default padding character used by
    @link(ATextOutputStream.printWrapped) in order to align each line to a
    specific column.
  }
  tosWrapDefaultPadCharacter = ' ';
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
  // Stores the difference between the bytes read and those needed
  difference: integer;

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
    difference := MyTarget.read(sizeof(longword), ch[1]);

    // Get the length of the first character in the string
    charLength := Charstring.characterLength(ch);

    // Copy that number of bytes to our destination
    dest := dest + System.copy(ch, 1, charLength);
    inc(result);

    // Rewind the stream by the difference
    MyTarget.rewindBy(difference - charLength);
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

// ATextOutputStream ***********************************************************
// Write to the stream
function ATextOutputStream.write(const count: TStreamIOSize;
  const source): TStreamIOSize;

begin
  result := 0;

  if MyTarget <> nil then
    result := MyTarget.write(count, source);
end;

{* Writes a string to the stream.  This method calls the abstract method
   AStream::write() to perform the actual writing.

   \return A value indicating the number of bytes written to the stream.  This
   value may be zero if the string was empty.
*}
function ATextOutputStream.print(const str: AnsiString): TStreamIOSize;

begin
  result := 0;
  if str <> '' then
    result := Self.write(System.length(str), str[1]);
end;

{* Converts each member of \p vals to a string and writes it to the stream.
   This method is primarily designed to make it easy to quickly output data
   from various sources to a console display or text file; the strings it
   produces are NOT preceded by length specifiers and so are unsuitable for
   later retrieval from the stream.

   For members of \p vals that are instances of AnObject which also implement
   CanPrint, this routine calls CanPrint::printTo() in order to produce
   output for that object.

   \return The total number of bytes written to the stream.  This value should
   be the sum total of the memory occupied by all the members of \p vals, but
   may be less than expected if an error occurred while writing.
*}
function ATextOutputStream.printEach(const vals: array of const): TStreamIOSize;

var
  // Loop counter
  i: longword;

begin
  result := 0;

  if system.length(vals) > 0 then
  begin
    for i := 0 to high(vals) do
    begin
      case vals[i].vtype of
        vtInteger:
          result := result + Self.print(intToStr(vals[i].vinteger));

        vtBoolean:
          if vals[i].vboolean then
            result := result + Self.print('true')

          else
            result := result + Self.print('false');

        vtChar:
          result := result + Self.print(vals[i].vchar);

        vtExtended:
          result := result + Self.print(floatToStr(vals[i].vextended^));

        vtString:
          result := result + Self.print(vals[i].vstring^);

        vtPointer:
          result := result + Self.print(intToHex(int64(vals[i].vpointer),
            (sizeof(int64) div 4)));

        vtPChar:
          result := result + Self.print(vals[i].vpchar^);

        vtObject:
          if (vals[i].vobject <> nil) and (vals[i].vobject is AnObject) then
            if AnObject(vals[i].vobject) is CanPrint then
              with vals[i].vobject as CanPrint do
                result := result + printTo(Self);

        vtClass:
          if vals[i].vclass <> nil then
            result := result + Self.print(vals[i].vclass.className)

          else
            result := result + Self.print('Class: NIL');

        vtAnsiString:
          result := result + Self.print(AnsiString(vals[i].vansistring));

        vtWideString:
          result := result + Self.print(WideString(vals[i].vWideString));

        vtUnicodeString:
          result := result + Self.print(UnicodeString(vals[i].vUnicodeString));

        vtInt64:
          result := result + Self.print(intToStr(vals[i].vInt64^));

        vtQWord:
          result := result + Self.print(intToStr(vals[i].vQWord^));
      end;
    end;
  end;
end;

// Print the selected item to the stream, wrapping as necessary
function ATextOutputStream.printWrapped(const str: AnsiString;
  const breakCharacters: array of string;
  thisLineLength: longword = 0;
  const firstLineStartColumn: longword = 0;
  const remainingLinesStartColumn: longword = 0;
  padCharacter: string = '';
  const lineBreakCharacter: string = System.lineEnding): TStreamIOSize;

var
  // Refers to the lines of wrapped text
  Lines: AStringList;

begin
  result := 0;

  if str = '' then
    exit;

  if thisLineLength = 0 then
    thisLineLength := Self.lineLength;

  if padCharacter = '' then
    padCharacter := tosWrapDefaultPadCharacter;

  Lines := AStringList.wrapString(str, breakCharacters, thisLineLength,
    firstLineStartColumn, remainingLinesStartColumn, padCharacter);
  if Lines = nil then
    exit;

  { We call 'Lines.LastItem.printTo' to bypass the default behavior of
    AStringList.printTo, which will include a description of the list itself.
  }
  result := Lines.LastItem.printTo(Self, '', lineBreakCharacter);
  Lines.free;
end;

// Properties ------------------------------------------------------------------
// Retrieve the maximum length of a line before it is forcibly wrapped
function ATextOutputStream.lineLength: longword;

begin
  result := tosDefaultLineLength;
end;
{$ENDIF}

