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
{ This class can be used to iterate over the data in a stream.

  It is possible to process the data contained by the stream in chunks of an
  arbitrary size (from one byte to 8 bytes) by calling
  @link(AStreamIterator.setSize).

  @link(AStream) and its descendants make use of this class to produce CRC
  checksums of stream data when @link(AStream.checksum) is called, as shown
  below: @longcode(#

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
  AStreamIterator = class(AnIterator)
  protected
    // Refers to the stream over which the iterator will iterate
    MyStream: AStream;
    // Stores the most recent value read from the stream
    myValue: qword;
    // Stores the size of the value read from the stream
    myValueSize: TStreamIOSize;

  public
    { Construct an instance of @link(AStreamIterator) that can be used to
      process the data in @code(ThisStream).

      The iterator will begin processing data at the current position in the
      given stream; this means that if @code(ThisStream) has already reached
      its end, the instance of @link(AStreamIterator) returned by this
      constructor may be useless until the underlying stream position pointer
      is reset.

      It is unlikely that you will call this routine directly; it is called
      automatically by @link(AStream.iterator).
    }
    constructor over(const ThisStream: AStream);

    { Initializes the iterator instance }
    function init: boolean; override;

    { Retrieves the next element from the stream.

      The size of the element read from the stream may be from one to eight
      bytes; this is determined by @link(AStreamIterator.size) and can be
      changed by calling @link(AStreamIterator.setSize) before calling this
      routine.  The value retrieved from the stream can be accessed by calling
      @link(AStreamIterator.value).
    }
    procedure next; override;

    { Determines whether or not there are more items to retrieve from the
      underlying stream.

      If the underlying stream has reached its end, this routine will return
      @false; otherwise, it returns @true.
    }
    function continues: boolean; override;

    { Construct a shallow copy of the given object.

      This method overrides the behavior inherited from
      @link(AnObject.shallowCopyFrom); it calls that routine, then ascertains
      whether @code(Other) is an instance of @link(AStreamIterator).  If it is,
      then it copies the current values of:
      @unorderedList(
        @item(@link(AStreamIterator.Stream),)
        @item(@link(AStreamIterator.value), and)
        @item(@link(AStreamIterator.size))
      )
      from @code(Other) to @code(Self), overriding any current values in
      @code(Self) before it returns @true.  Otherwise it returns @false.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

  // Properties ----------------------------------------------------------------
    { Retrieves a reference to the stream which is being traversed by the
      iterator.
    }
    function Stream: AStream; virtual;

    { Retrieves the most recent value read from the stream.

      This function returns a generic pointer to the most recent value read;
      this pointer can be cast to the appropriate type by the caller.  This
      value is volatile, which means it will be overwritten by the next call
      to @link(AStreamIterator.next).
    }
    function value: pointer; virtual;

    { Retrieves the size of values read from the stream.

      The value returned by this function will be from one to eight, indicating
      the number of bytes read from the underlying stream by each call to
      @link(AStreamIterator.next).  It is possible to change this value by
      calling @link(AStreamIterator.setSize).

      By default, instances of @link(AStreamIterator) read eight bytes of data
      at a time from the stream.
    }
    function size: TStreamIOSize; virtual;

    { Sets the size of the values read from the stream.

      @code(newSize) may be a value from one to eight, indicating that the
      iterator should read from one to eight bytes on the next call to
      @link(AStreamIterator.next).  Any other values are ignored.

      @return(The previous value of @link(AStreamIterator.size).)
    }
    function setSize(const newSize: TStreamIOSize): TStreamIOSize; virtual;
end;

const
  { The default size of values read by @link(AStreamIterator). }
  striDefaultSize = sizeof(int64);
{$ENDIF}
{$IFDEF ReadImplementation}
// AStreamIterator *************************************************************
/// Constructor
constructor AStreamIterator.over(const ThisStream: AStream);

begin
  inherited new;
  MyStream := ThisStream;
  { Load the first value }
  Self.next;
end;

/// Initializer
function AStreamIterator.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyStream := nil;
    myValue := 0;
    myValueSize := striDefaultSize;
  end;
end;

/// Retrieves the next byte value from the iterator
procedure AStreamIterator.next;

begin
  if (MyStream <> nil) and (MyStream.hasEnded = false) then
  begin
    myValue := 0;
    MyStream.read(myValueSize, myValue);
  end;
end;

/// Determines whether or not there is more of the stream over which to iterate
function AStreamIterator.continues: boolean;

begin
  result := false;

  if (MyStream <> nil) then
    result := (MyStream.hasEnded = false);
end;

// Construct a shallow copy of the given object
function AStreamIterator.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other item is compatible with us
  if not(Other is AStreamIterator) then
    result := false;

  if result then
  begin
    MyStream := AStreamIterator(Other).Stream;
    myValue := pqword(AStreamIterator(Other).value)^;
    myValueSize := AStreamIterator(Other).size;
  end;
end;

// Properties ------------------------------------------------------------------
/// Retrieves the stream used by the iterator
function AStreamIterator.Stream: AStream;

begin
  Result := MyStream;
end;

/// Retrieves the most recent value read from the stream
function AStreamIterator.value: pointer;

begin
  result := @myValue;
end;

/// Retrieve the size of values read from the stream
function AStreamIterator.size: TStreamIOSize;

begin
  result := myValueSize;
end;

/// Set the size of values read from the stream
function AStreamIterator.setSize(const newSize: TStreamIOSize): TStreamIOSize;

begin
  { Return the old size }
  result := myValueSize;

  { Ensure a valid value for our new size }
  if newSize in [sizeof(byte)..sizeof(myValue)] then
    myValueSize := newSize;
end;
{$ENDIF}

