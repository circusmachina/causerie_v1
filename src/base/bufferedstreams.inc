{ bufferedstreams.inc - Classes to buffer stream input and output.

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
const
  { This constant defines the default size, in bytes, for buffers used by
    instances of @link(ABufferedStream).  The default value is 4,096 bytes
    (4K).  Buffers are allocated on the heap.
  }
  bfstDefaultBufferSize = 4 * 1024;

// ABufferedStream *************************************************************
{ This class serves as the basis for a pair of classes that wrap around other
  streams and provide buffered input and output of those streams.  This class
  is not meant to be instantiated directly, as it does not implement the
  abstract methods inherited from @link(AStream); instead, you should use
  @link(ABufferedInputStream) to buffer input from another stream or
  @link(ABufferedOutputStream) to buffer output to another stream.
}
type ABufferedStream = class(AStreamWrapper)
  protected
    { Refers to the buffer used to buffer stream data }
    myBuffer: AnsiString;
    { Stores the current position within the buffer }
    myBufferPosition: pchar;
    { Refers to the end of the buffer }
    myBufferEnd: pchar;
    { Stores the position in the stream at which the buffer begins }
    myBufferStreamOffset: TStreamOffset;

    { Refresh the buffer.

      This method is meant to be implemented by descendant classes.  For
      descendants that support reading from the underlying stream, it should
      read from the stream into the buffer and return the number of bytes read.
      For descendants that support writing to the underlying stream, it should
      flush the buffer to the underlying stream and report the number of bytes
      written.
    }
    function refreshBuffer: TStreamIOSize; virtual; abstract;

  public
    { Construct a new buffered stream instance that will buffer input or
      output from the specified stream.

      If @code(takeOwnershipOfTarget) is @true, then the buffered stream
      instance will assume ownership of @code(Target) and will free the stream
      when it is itself freed.

      @code(bufferSize) specifies the size of the buffer to use, in bytes.
      If it is omitted or zero (0), then the default value specified by
      @link(bfstDefaultBufferSize) is used.  If memory for the buffer cannot
      be allocated, this method will raise an exception.

      @raises(ABufferedStreamError if a buffer could not be allocated.)
    }
    constructor around(const ThisTarget: AStream;
      const takeOwnershipOfTarget: boolean = true;
      const bufferSize: longword = bfstDefaultBufferSize); reintroduce; virtual;

    { Initializer }
    function init: boolean; override;

    { Free the buffered stream instance and the buffer it uses.

      If @link(AStreamWrapper.ownsTarget ABufferedStream.ownsTarget) is @true,
      then this method will free the target stream.
    }
    destructor destroy; override;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(AStreamWrapper.shallowCopyFrom): it calls that routine, then checks
      to see whether @code(Other) is an instance of @link(ABufferedStream).  If
      so, it copies the values of
      @unorderedList(
        @item(@link(ABufferedStream.buffer),)
        @item(@link(ABufferedStream.bufferPosition), and)
        @item(@link(ABufferedStream.myBufferStreamOffset))
      )
      from @code(Other) to @code(Self), overwriting any current values in
      @code(Self).

      @bold(NOTE:)
      The end result in this case is an almost-exact copy of @code(Other);
      however, the copy will @bold(not) assume ownership of the target stream,
      and read and write operations on the copy will @bold(not) propagate to
      @code(Other).  The copy's buffer will update independently of the
      original!  This means that if you read or write from the copy and then
      read or write from the original, you may end up with duplicate
      data (if both buffers have the same information), or you may confuse your
      your position in the source stream.

      This method should therefore be used with care.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the buffer used to buffer data for the target
      stream.  This buffer is allocated on the heap.

      The reference returned by this routine may be used to manipulate the
      contents of the buffer directly, but it should NOT be freed.  That will
      be done by the buffered stream instance when it is itself freed.
    }
    function buffer: pchar; virtual;

    { Retrieve the size, in bytes, of the buffer used to buffer data for the
      target stream.
    }
    function bufferSize: longword; virtual;

    { Retrieve a reference to the current position of the buffered stream
      within its buffer.  This is the position in the buffer at which the
      next read or write operation will occur.

      The reference returned by this routine may be used to manipulate the
      contents of the buffer directly, but it should NOT be freed.  That will
      be done by the buffered stream instance when it is itself freed.
    }
    function bufferPosition: pchar; virtual;

    { Retrieve the current position within the stream.

      This method calculates the current position in the stream by taking the
      value of @link(ABufferedStream.myBufferStreamOffset) and adding it to the
      current buffer position.
    }
    function position: TStreamOffset; override;
end;

// ABufferedInputStream ********************************************************
{ This class wraps around an input stream and buffers input from that stream. }
type ABufferedInputStream = class(ABufferedStream)
  protected
    { Refresh the buffer.

      This method reads from the underlying stream into the buffer and returns
      the total number of bytes read.  If the underlying stream indicates that
      the end of the stream has already been reached, then this method does
      nothing.

      @return(The total number of bytes read from the underlying stream.  If the
      end of the stream was reached before this method was called, then this
      value will be zero (0).)
    }
    function refreshBuffer: TStreamIOSize; override;

  public
    { Read @code(count) bytes from the buffer and store the data in @code(dest).

      This method retrieves data first from the buffer.  If the end of the
      buffer is reached before @code(count) bytes can be returned, then the
      target stream is queried for more data and the buffer is refilled, if
      possible.

      @return(The total number of bytes read from the stream.  This value may
      be less than @code(count) if the end of the stream was encountered while
      reading.)
    }
    function read(const count: TStreamIOSize; var dest): TStreamIOSize;
      override;

    { Rewind the stream position pointer by the specified number of bytes.

      If the number of bytes specified falls within the buffer, this method
      simply updates the buffer position.  Otherwise, it queries the stream for
      more information and the buffer is reloaded.

      @return(The previous position within the stream, in bytes relative to the
      beginning of the stream.)
    }
    function rewindBy(count: TStreamIOSize): TStreamOffset; override;

    // Properties --------------------------------------------------------------
    { Set the current position within the stream.

      This method sets the current position in the stream.  If the position
      desired is within the buffer, then this method simply updates the current
      buffer position.  Otherwise, the underlying stream is queried for more
      data and the buffer is reloaded, beginning at the position specified by
      @code(newPosition).

      As with the inherited routine, @code(newPosition) may be a positive or
      negative value.  If positive, @code(newPosition) refers to a position,
      in bytes, relative to the beginning of the stream.  The first element in
      the stream is always at position zero (0).  If negative,
      @code(newPosition) refers to a location, in bytes, relative to the end of
      the stream.  The last element in the stream is always at position -1.

      @return(The previous position within the stream, relative to the beginning
      of the stream.)
    }
    function setPosition(newPosition: TStreamOffset): TStreamOffset; override;
end;

// ABufferedOutputStream *******************************************************
{ This class wraps around an output stream and buffers output to that stream.
  Before the buffered stream instance is destroyed, any data remaining in the
  buffer will be flushed to the target stream.
}
type ABufferedOutputStream = class(ABufferedStream)
  protected
    { Refresh the buffer.

      This method flushes the contents of the buffer to the underlying stream,
      then zeroes the buffer for use in future write operations.

      @return(The total number of bytes written to the underlying stream.)
    }
    function refreshBuffer: TStreamIOSize; override;

  public
    { Free the stream and the buffer associated with it.

      This method overrides the behavior inherited from @link(ABufferedStream).
      It flushes any data remaining in its buffer to the target stream before
      calling the inherited routine.
    }
    destructor destroy; override;

    { Write @code(count) bytes of @code(source) to the stream.

      It is the caller's responsibility to ensure that @code(source) has at
      least @code(count) bytes ready to be written; failure to do so will
      likely cause your code to crash.

      This method writes data to the buffer.  If the end of the buffer is
      reached before @code(count) bytes have been written, then the contents
      of the buffer are flushed to the target stream before writing continues.

      @return(The total number of bytes written to the stream.)
    }
    function write(const count: TStreamIOSize; const source): TStreamIOSize;
      override;

    // Properties --------------------------------------------------------------
    { Set the current position within the stream.

      This method sets the current position in the stream.  If the position
      desired is within the buffer, then this method simply updates the current
      buffer position.  Otherwise, the buffer is flushed and the underlying
      stream moved to the new position.

      As with the inherited routine, @code(newPosition) may be a positive or
      negative value.  If positive, @code(newPosition) refers to a position,
      in bytes, relative to the beginning of the stream.  The first element in
      the stream is always at position zero (0).  If negative,
      @code(newPosition) refers to a location, in bytes, relative to the end of
      the stream.  The last element in the stream is always at position -1.

      @return(The previous position within the stream, relative to the beginning
      of the stream.)
    }
    function setPosition(newPosition: TStreamOffset): TStreamOffset; override;
end;

// ABufferedStreamError ********************************************************
{ This class represents an error that may occur when attempting to allocate the
  buffer for a buffered stream.  It will be thrown by
  @link(ABufferedStream.around) in the event that allocating the buffer fails.
}
type ABufferedStreamError = class(AnException);

{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string contains the error message used when
    @link(ABufferedStream.around) fails to allocate a buffer to use in
    buffering input from or output to a stream.

    The integer placeholder is filled with the desired size, in bytes, of the
    buffer that was to be allocated.  This value may be passed to the
    constructor; if it is omitted, then default value specified by
    @link(bfstDefaultBufferSize) is used.

    The string placeholder is filled with the name of the class, as returned by
    a call to @link(AnObject.name).
  }
  bfstErrorAllocatingBuffer = 'failed to allocate the %d bytes needed to ' +
    'buffer data for %s';
{$ENDIF}
{$IFDEF ReadImplementation}
// ABufferedStream *************************************************************
// Constructor
constructor ABufferedStream.around(const ThisTarget: AStream;
  const takeOwnershipOfTarget: boolean = true;
  const bufferSize: longword = bfstDefaultBufferSize);

begin
  inherited around(ThisTarget, takeOwnershipOfTarget);

  try
    System.setLength(myBuffer, bufferSize);
    Self.refreshBuffer;

  except
    raise ABufferedStreamError.because(bfstErrorAllocatingBuffer, [
      bufferSize, Self.name
    ]);

  end;
end;

// Initializer
function ABufferedStream.init: boolean;

begin
  result := inherited init;

  if result then
  begin;
    myBuffer := '';
    myBufferPosition := nil;
    myBufferEnd := nil;
    myBufferStreamOffset := 0;
  end;
end;

// Destructor
destructor ABufferedStream.destroy;

begin
  // Free the buffer
  System.setLength(myBuffer, 0);

  inherited destroy;
end;

// Construct a shallow copy of the given object
function ABufferedStream.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other item is compatible with us
  if not(Other is ABufferedStream) then
    result := false;

  if result then
  begin
    // Copy the buffer
    myBuffer := ABufferedStream(Other).myBuffer;
    myBufferPosition := ABufferedStream(Other).myBufferPosition;
    myBufferStreamOffset := ABufferedStream(Other).myBufferStreamOffset;
  end;
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the buffer used to buffer data
function ABufferedStream.buffer: pchar;

begin
  result := nil;

  if System.length(myBuffer) > 0 then
    result := @myBuffer[1];
end;

// Retrieve the size, in bytes, of the buffer used to buffer data
function ABufferedStream.bufferSize: longword;

begin
  result := System.length(myBuffer);
end;

// Retrieve the current position within the buffer
function ABufferedStream.bufferPosition: pchar;

begin
  result := myBufferPosition;
end;

// Get the current position in the underlying stream
function ABufferedStream.position: TStreamOffset;

begin
  result := myBufferStreamOffset + (myBufferPosition - @myBuffer[1]);
end;

// ABufferedInputStream ********************************************************
// Refresh the buffer
function ABufferedInputStream.refreshBuffer: TStreamIOSize;

begin
  result := 0;

  if (MyTarget = nil) or (MyTarget.hasEnded) then
    exit;

  myBufferStreamOffset := MyTarget.position;

  // Clear the buffer
  MemorySpan.fill(@myBuffer[1], Self.bufferSize, 0);
  // Read into the buffer
  result := MyTarget.read(Self.bufferSize, myBuffer[1]);

  { This ensures that, if less data is returned than will fill the entire
     buffer, we do not end up reading beyond that and returning "junk" data
     from previous reads.
  }
  myBufferEnd := @myBuffer[result];
  myBufferPosition := @myBuffer[1];
end;

// Read from the buffer
function ABufferedInputStream.read(const count: TStreamIOSize;
  var dest): TStreamIOSize;

var
  // Refers to the output
  output: pchar;
  // Stores the number of bytes remaining to read
  remaining: TStreamIOSize;
  // Stores the size of a buffer span
  spanSize: TStreamIOSize;

begin
  result := 0;

  if (MyTarget = nil) or (count = 0) then
    exit;

  remaining := count;
  output := @dest;

  while remaining > 0 do
  begin
    // Read as much as possible from the buffer
    spanSize := myBufferEnd - myBufferPosition;

    // But don't read more than is available
    if spanSize > remaining then
      spanSize := remaining;

    // Store the data in the output
    MemorySpan.copy(myBufferPosition, output, spanSize);
    dec(remaining, spanSize);
    inc(myBufferPosition, spanSize);
    inc(output, spanSize);
    inc(result, spanSize);

    // Read more data into the buffer, if necessary
    if myBufferPosition > myBufferEnd then
    begin
      spanSize := Self.refreshBuffer;

      // If no data was read, then we exit
      if spanSize = 0 then
        break;
    end;
  end;
end;

// Rewind the stream position pointer
function ABufferedInputStream.rewindBy(count: TStreamIOSize): TStreamOffset;

var
  // Stores the new position in the stream
  newPosition: TStreamOffset;

begin
  // Get the current position in the stream
  result := Self.position;

  if MyTarget = nil then
    exit;

  // Check to see if we are within the current buffer
  if (myBufferPosition - count) < myBufferEnd then
  begin
    // We are, so we simply update the buffer position
    dec(myBufferPosition, count);
    exit;
  end;

  { Otherwise, reload the buffer such that the desired position is at the
    midpoint of the buffer: current stream position, less count, less
    half the size of the buffer.
  }
  newPosition := (result - count) - (Self.bufferSize div 2);

  { But not beyond the beginning of the stream }
  if newPosition < 0 then
    newPosition := 0;

  MyTarget.setPosition(newPosition);
  { Update our position: midway point of buffer, exactly where we need to be }
  inc(myBufferPosition, Self.bufferSize div 2);
end;

// Properties ------------------------------------------------------------------
// Set the current position within the stream
function ABufferedInputStream.setPosition(newPosition: TStreamOffset): TStreamOffset;

begin
  result := Self.position;

  if MyTarget = nil then
    exit;

  // Adjust for negative offsets
  if newPosition < 0 then
    inc(newPosition, MyTarget.length + 1);

  if newPosition < 0 then
    newPosition := 0;

  // Check to see if the new position is within the buffer
  if (newPosition >= myBufferStreamOffset) and
    (newPosition <= (myBufferStreamOffset + Self.bufferSize)) then
    myBufferPosition := @myBuffer[newPosition - myBufferStreamOffset + 1]

  else begin
    // Set the new position in the underlying stream
    MyTarget.setPosition(newPosition);
    // Refresh the buffer
    Self.refreshBuffer;
  end;
end;

// ABufferedOutputStream *******************************************************
// Destructor
destructor ABufferedOutputStream.destroy;

begin
  { Flush any remaining data in the buffer }
  Self.refreshBuffer;

  inherited destroy;
end;

// Refresh the buffer
function ABufferedOutputStream.refreshBuffer: TStreamIOSize;

begin
  result := 0;

  if (MyTarget = nil) or (Self.bufferSize = 0) then
    exit;

  { Check to see if there is anything to write }
  if myBufferPosition = @myBuffer[1] then
    exit;

  result := MyTarget.write((myBufferPosition - @myBuffer[1]), myBuffer[1]);
  myBufferStreamOffset := MyTarget.position;

  // Clear the buffer
  MemorySpan.fill(@myBuffer[1], Self.bufferSize, 0);
  myBufferPosition := @myBuffer[1];
end;

// Write to the buffer
function ABufferedOutputStream.write(const count: TStreamIOSize;
  const source): TStreamIOSize;

var
  // Refers to the input
  input: pchar;
  // Stores the number of bytes remaining to write
  remaining: TStreamIOSize;
  // Stores the size of a buffer span
  spanSize: TStreamIOSize;

begin
  result := 0;

  if (MyTarget = nil) or (count = 0) then
    exit;

  remaining := count;
  input := @source;

  while remaining > 0 do
  begin
    // Write as much as possible to the buffer
    spanSize := myBufferEnd - myBufferPosition;

    // But don't write more than necessary
    if spanSize > remaining then
      spanSize := remaining;

    // Write the input data
    MemorySpan.copy(input, myBufferPosition, spanSize);
    dec(remaining, spanSize);
    inc(myBufferPosition, spanSize);
    inc(input, spanSize);
    inc(result, spanSize);

    // Flush the buffer, if necessary
    if myBufferPosition > myBufferEnd then
      Self.refreshBuffer;
  end;
end;

// Properties ------------------------------------------------------------------
// Set the current position within the stream
function ABufferedOutputStream.setPosition(newPosition: TStreamOffset): TStreamOffset;

begin
  result := Self.position;

  if MyTarget = nil then
    exit;

  // Correct for negative offsets
  if newPosition < 0 then
    inc(newPosition, MyTarget.length + 1);

  if newPosition < 0 then
    newPosition := 0;

  // Check to see whether the new position is within the buffer
  if (newPosition >= myBufferStreamOffset) and
    (newPosition <= (myBufferStreamOffset + Self.bufferSize)) then
    myBufferPosition := @myBuffer[newPosition - myBufferStreamOffset]

  else begin
    // Refresh the buffer
    Self.refreshBuffer;
    // Set the new position in the underlying stream
    MyTarget.setPosition(newPosition);
    myBufferStreamOffset := MyTarget.position;
  end;
end;

{$ENDIF}
