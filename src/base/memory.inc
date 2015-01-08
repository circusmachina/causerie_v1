{ classwork.inc - Memory utility routines.

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
{ This is a utility class that provides some helpful routines for quickly
  filling and copying stretches of memory.
}
type MemorySpan = class
  public
    {$IF DEFINED(CPU32)}
    { Quickly fill a block of memory with a specific value.

      This method fills the memory at @code(dest) with the value specified by
      @code(value), until it has filled @code(len) bytes.  It speeds up the
      process by using the native word size on the processor for which it was
      compiled; thus, for 32-bit systems it fills memory 4 bytes at a time,
      while on 64-bit systems it fills memory 8 bytes at a time.

      @code(len) must always specify the number of @bold(bytes) to fill,
      independent of the native word size of the processor.

      It is the caller's responsibility to ensure that the memory block pointed
      to by @code(dest) has at least @code(len) bytes allocated to it;
      otherwise, memory overruns and segmentation faults may occur.
    }
    class procedure fill(dest: pointer; const len: longword;
      const value: longword);
  {$ELSEIF DEFINED(CPU64)}
    { Quickly fill a block of memory with a specific value.

      This method fills the memory at @code(dest) with the value specified by
      @code(value), until it has filled @code(len) bytes.  It speeds up the
      process by using the native word size on the processor for which it was
      compiled; thus, for 32-bit systems it fills memory 4 bytes at a time,
      while on 64-bit systems it fills memory 8 bytes at a time.

      @code(len) must always specify the number of @bold(bytes) to fill,
      independent of the native word size of the processor.

      It is the caller's responsibility to ensure that the memory block pointed
      to by @code(dest) has at least @code(len) bytes allocated to it;
      otherwise, memory overruns and segmentation faults may occur.
    }
    class procedure fill(dest: pointer; const len: longword;
      const value: qword);
  {$ENDIF}

    { Quickly copy data from one memory block to another.

      This method quickly copies @code(len) bytes of data from @code(source)
      to @code(dest).  It speeds up the process by using the native word size
      on the processor for which it was compiled; thus, for 32-bit systems it
      copies 4 bytes at a time, while on 64-bit systems it copies 8 bytes at
      a time.

      @code(len) must always specify the number of @bold(bytes) to copy,
      independent of the native word size of the processor.  It is also
      assumed that the memory regions pointed to by @code(source) and
      @code(dest) do @bold(NOT) overlap; if they do, this method may produce
      undefined results, up to and including segmentation faults.

      It is the caller's responsibility to ensure that the memory blocks pointed
      to by both @code(source) and @code(dest) have at least @code(len) bytes
      allocated to them; otherwise, memory overruns and segmentation faults
      may occur.
    }
    class procedure copy(source, dest: pointer; const len: longword);

    { Determine the number of bytes that will need to be added to @code(current)
      so that it is aligned on the specified boundary relative to @code(top).

      This method can be used to ensure that data read from a buffer is aligned
      to a specific address size.  @code(current) should point to the current
      position in the buffer, while @code(top) points to the "top" of the
      buffer -- that is, the first byte in the buffer.

      @code(alignment) should specify the alignment required, in bytes.  An
      8-bit alignment would require a value of 1; a 16-bit alignment a value of
      2; a 32-bit alignment a value of 4; and so forth.  If the memory position
      of @code(current) does divide evenly into the specified @code(alignment),
      then this method will calculate how many bytes need to be added to
      @code(alignment) so that it will divide evenly.  This value is then
      multiplied by @code(increment).

      For example, Windows version information is stored in pseudo-structures,
      the members of which must sometimes be aligned on a 32-bit address
      boundary using words (16-bit values).  To have this method calculate the
      padding necessary to meet this requirement, you would call this method
      like so: @longcode(#

      var
        buffer: pchar;
        currentPos: pchar;

      begin
        // Assuming 'buffer' and 'currentPos' are valid addresses, and that
        // 'currentPos' represents a position within 'buffer'...
        inc(currentPos, MemorySpan.paddingRequired(currentPos, buffer,
          sizeof(longword), sizeof(word)));
        // 32 bits = 4 bytes, or sizeof(longword)
        // 16 bits = 2 bytes, or sizeof(word)
      end.
      #)

      If @code(current) or @code(top) are @nil, or if @code(current) does not
      represent a memory address which is greater than @code(top), then this
      method does nothing and returns zero (@code(0)).
    }
    class function paddingRequired(const current, top: pchar;
      alignment, increment: longword): longword;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
const
{$IF DEFINED(CPU32)}
  nativeWordSize = sizeof(longword);
{$ELSEIF DEFINED(CPU64)}
  nativeWordSize = sizeof(qword);
{$ENDIF}

{$IF DEFINED(CPU32)}
/// Quickly fills a block of memory with a specific value
class procedure MemorySpan.fill(dest: pointer; const len: longword;
  const value: longword);
{$ELSEIF DEFINED(CPU64)}
/// Quickly fills a block of memory with a specific value
class procedure MemorySpan.fill(dest: pointer; const len: longword;
  const value: qword);
{$ENDIF}

var
  // Loop counter
  i: longword;
  // Loop maximum
  loopMax: longword;

begin
  if len = 0 then exit;

  // Move 4 or 8 bytes at a time, so long as we have at least that many to move
  if len >= nativeWordSize then
  begin
    loopMax := len div nativeWordSize;
    for i := 1 to loopMax do
    begin
    {$IF DEFINED(CPU32)}
      plongword(dest)^ := value;
    {$ELSEIF DEFINED(CPU64)}
      pqword(dest)^ := value;
    {$ENDIF}
      inc(dest, nativeWordSize);
    end;
  end;

  // Finish off any straggling bytes
  loopMax := len mod nativeWordSize;
  if loopMax > 0 then
    for i := 1 to loopMax do
    begin
      pbyte(dest)^ := byte(value);
      inc(dest, sizeof(byte));
    end;
end;

/// Quickly copies from one block of memory to another
class procedure MemorySpan.copy(source, dest: pointer; const len: longword);

var
  // Loop counter
  i: longword;
  // Loop maximum
  loopMax: longword;

begin
  if len = 0 then exit;

  // Copy 4 or 8 bytes at a time, so long as we have at least that many to copy
  if len >= nativeWordSize then
  begin
    loopMax := len div nativeWordSize;
    for i := 1 to loopMax do
    begin
    {$IF DEFINED(CPU32)}
      plongword(dest)^ := plongword(source)^;
    {$ELSEIF DEFINED(CPU64)}
      pqword(dest)^ := pqword(source)^;
    {$ENDIF}
      inc(source, nativeWordSize);
      inc(dest, nativeWordSize);
    end;
  end;

  // Copy any straggling bytes
  loopMax := len mod nativeWordSize;
  if loopMax > 0 then
    for i := 1 to loopMax do
    begin
      pbyte(dest)^ := pbyte(source)^;
      inc(source, sizeof(byte));
      inc(dest, sizeof(byte));
    end;
end;

// Determine the number of padding bytes required
class function MemorySpan.paddingRequired(const current, top: pchar;
  alignment, increment: longword): longword;

begin
  result := 0;

  if (current <> nil) and (top <> nil) and (current > top) then
  begin
    while (((current - top) + result) mod alignment) <> 0 do
      inc(result, increment);
  end;
end;

{$ENDIF}
