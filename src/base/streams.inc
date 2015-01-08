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
type
  { The type of value used to indicate the size of data to read or write from
    the stream.  This type is declared to increase the flexibility of stream
    code.
  }
  TStreamIOSize = longword;

  { The type of value used to indicate the overall size of a stream. This type
    is declared to increase the flexibility of stream code.
  }
  TStreamLength = int64;

  { The type of value used to indicate an offset within a stream. This type
    is declared to increase the flexibility of stream code.
  }
  TStreamOffset = int64;

  { The type of value used when calculating a checksum for a stream. This type
    is declared to increase the flexibility of stream code.
  }
  TStreamChecksum = int64;

{ The base stream type.

  This class represents a basic binary stream that supports reading, writing,
  and random access.  The class defines several abstract methods which its
  descendants are expected to implement:
  @unorderedList(
    @item(@link(AStream.read))
    @item(@link(AStream.write))
    @item(@link(AStream.position))
    @item(@link(AStream.setPosition))
    @item(@link(AStream.length))
  )
  as well as several utility routines.

  Because this class represents an abstract stream, it is not meant to be
  directly instantiated; instead, you will use one of its descendants.  To
  write to a disk-based file, you will use @link(AFileOutputStream) or one
  of its descendants.  To read from a disk-based file, you will use
  @link(AFileInputStream) or one of its descendants.  To read and write from
  a memory-based stream, you can use @link(AStringStream) or one of its
  descendants.

  All streams are expected to implement the @link(CanIterate) interface.  This
  class provides a default implementation, which returns an instance of
  @link(AStreamIterator).  @link(AStream) itself uses this interface to
  implement @link(AStream.checksum), as shown here: @longcode(#

  // Calculate a checksum for the stream
  function AStream.checksum: TStreamChecksum;

  var
    // Stores the size of the stream
    streamSize: TStreamLength;
    // Stores the position in the stream before the checksum was calculated
    previousPosition: TStreamOffset;
    // Used to iterate over the stream
    Element: AStreamIterator;

  begin
    result := 0;
    streamSize := Self.length;
    if streamSize > 0 then
    begin
      // Store the current position
      previousPosition := Self.position;

      Element := AStreamIterator(Self.iterator);
      result := crc64(0, nil, 0);

      while Element.continues do
      begin
        result := crc64(result, Element.value, Element.size);
        Element.next;
      end;

      Element.free;
      // Seek the previous position
      Self.setPosition(previousPosition);
    end;
  end;
  #)
}
type
  AStream = class(AnObject, CanIterate)
  public
    { Read @code(count) bytes from the stream and store the data in
      @code(dest).

      This method is to be implemented by descendant classes.  It should
      obtain the desired amount of data, if possible, and store it in the
      memory specified by @code(dest).

      This method may throw an exception if the read fails, but it should NOT
      throw an exception if the end of the stream is encountered; instead, it
      should simply return the total number of bytes that were read before the
      end of the stream was reached.

      It is the caller's responsibility to ensure that @code(dest) has at least
      @code(count) bytes available for storage; failure to do so will likely
      cause your program to crash.

      @return(The total number of bytes read from the stream.  This value may
      be less than @code(count) if the end of the stream was encountered while
      reading.)
    }
    function read(const count: TStreamIOSize; var dest): TStreamIOSize; virtual;
      abstract;

    { Write @code(count) bytes of @code(source) to the stream.

      This method is to be implemented by descendant classes.  It should
      read the desired amount of data from @code(source) and output it to the
      stream.

      This method may throw an exception if the write fails.

      It is the caller's responsibility to ensure that @code(source) has at
      least @code(count) bytes ready to be written; failure to do so will
      likely cause your code to crash.

      @return(The total number of bytes written to the stream.)
    }
    function write(const count: TStreamIOSize; const source): TStreamIOSize;
      virtual; abstract;

    { Write each value in @code(vals) to the stream.

      This method is defined and implemented as a convenience function; it
      writes each variable provided in @code(vals) to the stream by calling
      @link(AStream.write) on each.  The type information encoded by the
      compiler is used to determine the size of each value to write.  The
      values are written as-is, in binary form, making this method unsuitable
      for printing text output.

      It is possible to pass instances of objects to this routine, but they
      must be members of a class that implements @link(CanStream).  This routine
      will call the @code(writeTo) method for all such instances passed.  All
      other object instances passed to this routine are ignored.

      If a class reference is passed to this routine, it will output a string
      containing the name of the class.

      @return(The total number of bytes written to the stream.  This value may
      be less than expected if certain items in @code(vals) were skipped.)
    }
    function writeEach(const vals: array of const): TStreamIOSize; virtual;

    { Write a string to the stream.

      This method first writes the length specifier of @code(str), then writes
      a checksum for that length before writing the text of @code(str) itself.

      The checksum is written as a precaution, to help ensure that an invalid
      string can be detected when reading from the stream before an attempt is
      made to read it.  This can prevent memory errors, since otherwise it would
      be possible to position the stream at an arbitrary location and attempt
      to read a string, with undefined results.  The checksum written is simply
      the negative value of the string's length.  For instance, if @code(str)
      is 255 bytes in length, the checksum written will be -255.

      The string written by this routine can be recalled from the stream by
      calling @link(AStream.readString).

      @bold(Note:) The length specifier and checksum written will always be
      of type @link(TStreamIOSize) (currently unsigned 32-bit integer values).

      Because it writes the binary value of the length specifier to the stream,
      this method is not suitable for simply printing a string for display to
      an end-user.

      @return(The total number of bytes printed to the stream.  This value
      should be the length of @code(str) plus the length of its length
      specifier and checksum.)
    }
    function writeString(const str: AnsiString): TStreamIOSize; virtual;

    { Read a string from the stream.

      This method first reads the length specifier and checksum from the
      stream.  If the negative value of the checksum matches that of the length
      specifier, then the text of the string itself is read from the stream; if
      not, then this routine will set @code(str) to an empty string and exit.

      @bold(Note:) The length specifier and checksum read will always be of
      type @link(TStreamIOSize) (currently unsigned 32-bit integer values).

      @return(The total number of bytes read from the stream.  This value should
      be the length of the string, plus the length of its length specifier and
      checksum.  In the event that the checksum and length specifier do not
      match as described above, the value returned by this routine will be
      exactly the size of the checksum and length specifier (currently 8
      bytes).)
    }
    function readString(var str: AnsiString): TStreamIOSize; virtual;

    { Rewinds the stream position pointer by @code(count) bytes.  Subsequent
      reads from or writes to the stream will occur at the new position.

      This method is useful when it is necessary to read from the stream and
      then ensure that the same data is read again.

      The absolute value of @code(count) is used.  If @code(count) represents
      a value that is greater than the current offset within the stream (in
      other words, if the stream position pointer would end up as a negative
      number) then the stream position pointer is reset to the beginning of the
      stream.

      @return(The previous position within the stream, as an offset in bytes
      from the beginning of the stream.)
    }
    function rewindBy(count: TStreamIOSize): TStreamOffset; virtual;

    { Calculate a 64-bit CRC checksum for the stream.

      The base implementation of this method starts at the current position
      within the stream and iterates over all bytes encountered until the
      end of the stream.  It then returns the stream position pointer to where
      it was when the method was called, and returns the checksum calculated.

      The reason the method starts at the @italic(current position) within the
      stream, instead of at the beginning of the stream, is to allow for a
      stream header that will contain the calculated checksum.  Since the
      checksum value in the stream header is undefined, it should not be
      included in the checksum calculation, or subsequent attempts to verify
      the stream against the checksum will fail.

      Descendant classes may override this method to introduce different
      behavior.

      @return(The 64-bit CRC checksum for the stream.)
    }
    function checksum: TStreamChecksum; virtual;

    // CanIterate implementation -----------------------------------------------
    { Construct and return an iterator suitable for processing the stream.

      The iterator returned by the base implementation of this routine will be
      an instance of @link(AStreamIterator), which allows access to the
      individual bytes that make up the stream.  Descendant classes may override
      this method to return a different iterator type.
    }
    function iterator: AnIterator; virtual;

  // Properties ----------------------------------------------------------------
    { Return the current position within the stream.

      This method should be implemented by descendant classes.  It should
      return the value of the stream position pointer, which indicates where
      the next read or write operation will take place, as an offset in bytes
      from the beginning of the stream.  The beginning of the stream should
      always be represented as offset zero (0).
    }
    function position: TStreamOffset; virtual; abstract;

    { Set the current position within the stream.

      This method should be implemented by descendant classes.  It should
      set the stream position pointer to @code(newPosition) and return the
      previous value of the position pointer.

      @code(newPosition) may be a positive or negative value.  If positive,
      it represents an offset in bytes from the beginning of the stream,
      where offset zero (0) represents the beginning of the stream.  If
      @code(newPosition) is a negative value, it represents an offset in bytes
      from the end of the stream, where offset -1 represents the end of the
      stream.

      This method should not throw an exception if the underlying mechanism
      does not support repositioning; it should instead fail silently and
      return the current, unchanged position within the stream.
    }
    function setPosition(newPosition: TStreamOffset): TStreamOffset; virtual;
      abstract;

    { Retrieve the current length of the stream.

      This method should be implemented by descendant classes.  It must return
      the current size of the stream, in bytes.
    }
    function length: TStreamLength; virtual; abstract;

    { Determine whether or not the end of the stream has been reached.

      The base implementation of this method simply checks to see whether
      @link(AStream.position) is greater than or equal to @link(AStream.length);
      if so, it returns @true.  Descendant classes may override this method in
      order to take advantage of another mechanism instead.

      @return(@true if the end of the stream has been reached; @false if not.)
    }
    function hasEnded: boolean; virtual;
  end;

{ This interface declares the methods that must be implemented by any class
   that is capable of streaming its data.
}
type CanStream = interface
  { Determine the size of the data that will be streamed, in bytes.

    Classes that implement this method should calculate the total size of
    the data that will be read from or written to a stream when
    either @link(CanStream.readFrom) or @link(CanStream.writeTo) is called.
  }
  function streamingLength: TStreamIOSize;

  { Reads class data from \p Source.  The class itself is responsible for
    validating that the information being read is pertinent to that class.
    This method is normally implemented to reverse the steps taken in
    @link(CanStream.writeTo).

    \return The number of bytes read from \p Source.  This value may be zero
    (0) if \p Source is @nil or the data to be read from \p Source is not
    pertinent to the class.
  }
  function readFrom(const Source: AStream): TStreamIOSize;

  { Writes class data to \p Dest.  The nature and meaning of this data is
    defined by the class; usually, however, a class will implement this method
    in a way that will allow @link(CanStream.readFrom) to recall the information
    from the stream (by including length specifiers, checksums, etc.).

    \return The number of bytes written to \p Dest.  This value will be zero
    (0) if \Dest is @nil or an error occurred while writing.
  }
  function writeTo(const Dest: AStream): TStreamIOSize;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
{* Writes the list of \p vals to the stream.  The values are written as they
   are, so this function is not suitable for producing output to a console or
   text file unless every member of \p vals is a string or character.  To
   produce output from a list of values that is suitable for a console display
   or text file, call AStream::writeStringsFrom().

   For members of \p vals that are instances of AnObject which also implement
   CanStream, this routine calls CanStream::writeTo() in order to produce
   output for that object.

   \return The total number of bytes written to the stream, which should be the
   total amount of memory occupied by each member of \p vals.  If an error
   occurs while writing to the stream, this value may be less than expected.
*}
function AStream.writeEach(const vals: array of const): TStreamIOSize;

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
          result := result + Self.write(sizeof(vals[i].vinteger),
            vals[i].vinteger);

        vtBoolean:
          result := result + Self.write(sizeof(vals[i].vboolean),
            vals[i].vboolean);

        vtChar:
          result := result + Self.write(sizeof(vals[i].vchar),
            vals[i].vchar);

        vtWideChar:
          result := result + Self.write(sizeof(vals[i].vWideChar),
            vals[i].vWideChar);

        vtExtended:
          result := result + Self.write(sizeof(vals[i].vextended^),
            vals[i].vextended^);

        vtString:
          result := result + Self.writeString(vals[i].vstring^);

        vtPointer:
          result := result + Self.write(sizeof(pointer),
            vals[i].vpointer);

        vtPChar:
          result := result + Self.writeString(vals[i].vpchar^);

        vtPWideChar:
          result := result +
            Self.writeString(System.wideCharToString(vals[i].vpWideChar));

        vtObject:
          if (vals[i].vobject <> nil) and (vals[i].vobject is AnObject) then
            if AnObject(vals[i].vobject) is CanStream then
              with vals[i].vobject as CanStream do
                result := result + writeTo(Self);

        vtClass:
          if vals[i].vclass <> nil then
            result := result + Self.writeString(vals[i].vclass.className);

        vtAnsiString:
          result := result + Self.writeString(AnsiString(vals[i].vansistring));

        vtWideString:
          result := result + Self.writeString(WideString(vals[i].vWideString));

        vtUnicodeString:
          result := result +
            Self.writeString(UnicodeString(vals[i].vUnicodeString));

        vtInt64:
          result := result + Self.write(sizeof(int64), vals[i].vInt64^);

        vtQWord:
          result := result + Self.write(sizeof(qword), vals[i].vQWord^);
      end;
    end;
  end;
end;

{* Writes \p str to the stream after writing a 32-bit value that indicates the
   length of the string, in bytes.  AStream::readString() will read such a
   string back into memory.

   \return The total number of bytes written to the stream.  This value should
   include the length of the string, plus four bytes for the length specifier.
   If an error occurs while writing to the stream, this value may be less than
   expected.
*}
function AStream.writeString(const str: AnsiString): TStreamIOSize;

var
  // Stores the length of the string
  stringLength: TStreamIOSize;
  // Stores the length checksum
  lengthCheck: TStreamIOSize;

begin
  stringLength := system.length(str);
  lengthCheck := longword(-stringLength);
  result := Self.write(sizeof(stringLength), stringLength);
  result := result + Self.write(sizeof(lengthCheck), lengthCheck);

  if stringLength > 0 then
    result := result + Self.write(stringLength, str[1]);
end;

{* Reads a string written to the stream by AString::writeString() back into
   memory.  This routine first reads the 32-bit value the precedes the string,
   so that it knows how much memory to allocate for the string data.

   \return The total number of bytes read from the stream.  This value should
   include the length of the string, plus four bytes for the length specifier.
   If an error occurs while reading from the stream, this value may be less than
   expected.
*}
function AStream.readString(var str: AnsiString): TStreamIOSize;

var
  // Stores the length of the string
  stringLength: TStreamIOSize;
  // Stores the length checksum
  lengthCheck: TStreamIOSize;

begin
  stringLength := 0;
  lengthCheck := 0;

  result := Self.read(sizeof(stringLength), stringLength);
  result := result + Self.read(sizeof(lengthCheck), lengthCheck);

  if stringLength <> longword(-lengthCheck) then
    exit;

  system.setLength(str, stringLength);

  if stringLength > 0 then
    result := result + Self.read(stringLength, str[1]);
end;

{* Rewinds the stream by the specified number of bytes.  This method calls the
   abstract method AStream.setPosition to move the stream position pointer.

   \return The previous position within the stream.  This may be the same as
   the current position if \p count is zero -- in that case, no position
   change will occur.
*}
function AStream.rewindBy(count: TStreamIOSize): TStreamOffset;

var
  // Stores the desired position within the stream
  desiredPosition: TStreamOffset;

begin
  // Get the current position
  result := Self.position;

  if count <= Self.length then
  begin
    desiredPosition := result - count;
    Self.setPosition(desiredPosition);
  end

  else
    Self.setPosition(0);
end;

{* Calculates a checksum for the stream.  This value is simply the sum of all
   byte values within the stream, with no correction made for rollover.

   \return The sum of all byte values within the stream.  This value will be
   zero (0) if the stream is empty.
*}
function AStream.checksum: TStreamChecksum;

var
  // Stores the size of the stream
  streamSize: TStreamLength;
  // Stores the position in the stream before the checksum was calculated
  previousPosition: TStreamOffset;
  // Used to iterate over the stream
  Element: AStreamIterator;

begin
  result := 0;
  streamSize := Self.length;

  if streamSize > 0 then
  begin
    // Store the current position
    previousPosition := Self.position;

    Element := AStreamIterator(Self.iterator);

    result := crc64(0, nil, 0);
    while Element.continues do
    begin
      result := crc64(result, Element.value, Element.size);
      Element.next;
    end;

    Element.free;
    // Seek the previous position
    Self.setPosition(previousPosition);
  end;
end;

// IsIterable implementation ---------------------------------------------------
/// Retrieve an iterator suitable for iterating through the stream
function AStream.iterator: AnIterator;

begin
  Result := AStreamIterator.over(Self);
end;

// Properties ------------------------------------------------------------------
{* Determines whether or not the end of the stream has been reached.  This
   function simply determines whether AStream::position() is greater than or
   equal to AStream::length(); it can be overridden by descendant classes that
   provide a better method for determining their ends.

   \return #true if the current position within the stream is greater than or
   equal to the overall size of the stream; #false if not.
*}
function AStream.hasEnded: boolean;

begin
  result := Self.position >= Self.length;
end;
{$ENDIF}


