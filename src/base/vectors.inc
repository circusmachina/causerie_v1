{ vectors.inc: A class to manage a dynamically-allocated block of memory
    that contains elements which are all the same size.

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
// AVector *********************************************************************
{ This class represents a vector, which is a dynamically-allocated
  one-dimensional array that contains elements which are all the same size.  The
  class acts in a fashion similar to the dynamic arrays offered by Free Pascal,
  and can also read itself from and write itself to an arbitrary stream.

  As with Free Pascal's dynamic arrays, vectors are zero-indexed, meaning the
  first element in the array is always at index zero (0).  The last element in
  the array is always at index @code(AVector.length - 1), and can also be
  referenced by calling @link(AVector.lastElement).

  It is unlikely that you will create a direct instance of this class, since
  its functionality is fairly basic.  Instead, you will most likely derive a
  class from it that adds the functionality required, or use an instance of
  @link(APointerVector) or @link(AnObjectVector).
}
type AVector = class(APrintingObject, CanStream)
  protected
    { Refers to the memory block that stores the items in the array }
    myBuffer: pchar;
    { Indicates the current size of the memory block that stores elements }
    myBufferLength: longword;
    { Indicates the size of elements in the array }
    myElementSize: longword;
    { Indicates the current number of elements in the array }
    myLength: longword;

  public
    { Construct a new instance of @link(AVector) that will handle elements
      which are of the given @code(size).

      The array will be sized to contain the @code(initialLength) elements, all
      of which will be @code(elementsSize) bytes in length.  If
      @code(initialValue) is specified, then all elements of the array will be
      set to the value it references.

      If specified, it is the caller's responsiblity to ensure that
      @code(initialValue) points to an item that is at least @code(size) bytes
      in length.  The value located there will be copied to all elements of the
      array.  If @code(initialValue) is @nil, then the array will be
      zero-filled.

      @raises(AVectorAllocationError if the initial attempt to allocate
      memory for the buffer fails.)
    }
    constructor ofLength(const initialLength: longword;
      const elementsSize: longword; const initialValue: pointer = nil); virtual;

    { Initializer }
    function init: boolean; override;

    { Destroy the managed array and the data it contains.

      If @link(AVector.buffer) contains pointers to instances of objects,
      these objects should be freed BEFORE this method is called, as this method
      does not free them.  Alternatively, you may use an instance of
      @link(AnObjectVector) and set its @code(ownsObjects) property to
      @true to ensure that object instances are freed when the array is
      destroyed.
    }
    destructor destroy; override;

    { Allocate memory to hold array elements.

      This method calls @code(System.reallocMem) to obtain a memory block that
      is large enough to hold @code(elementCount) elements that are all
      @link(AManagedArray.elementSize) bytes in length.  If the array already
      has a buffer allocated to it, then this method will attempt to increase
      the size of the array to include the new elements while preserving the
      existing elements.

      If @code(elementCount) specifies a value that is less than or equal to
      the current value of @link(AVector.length), then this routine does
      nothing.

      This method is called by @link(AVector.ofLength) to perform the initial
      allocation of the array.  It is also called by @link(AVector.grow) to
      extend the size of the array and by @link(AVector.readFrom) to obtain
      enough memory in order to read array elements from a given stream.

      @raises(AVectorAllocationError if a memory block large enough to
        hold the number of elements specified.)
    }
    procedure allocateElements(const elementCount: longword); virtual;

    { Grow the array by the specified number of elements.

      This method is called automatically whenever one or more new elements must
      be added to the array, but the value of @link(AVector.bufferLength)
      indicates there is not enough room to do so.  It attempts to increase the
      size of the buffer so it can contain @code(newElementCount) items.

      If @code(newElementCount) is zero (0), then the method will attempt to
      increase the size of the buffer by 1.5 times its current size.  The new
      elements are zero-filled by this method.

      This method calls @link(AVector.allocateElements).

      @raises(AVectorAllocationError if the attempt to reallocate the
      buffer failed.)
    }
    procedure grow(newElementCount: longword = 0); virtual;

    { Fill the entire array with a copy of the specified value.

      This method fills the array with the value specified by
      @code(sourceValue).  It is the caller's responsibility to ensure that
      @code(sourceValue) points to an item that is at least
      @link(AVector.elementSize) bytes in length.

      If @code(sourceValue) is @nil, each element in the array is filled with
      zero bytes; thus, calling this method with a @nil pointer effectively
      zeroes the array.
    }
    procedure fillElements(const sourceValue: pointer); virtual;

    { Push an element onto the array as the last item in the array.

      If necessary, the array will grow to accomodate the new item.

      It is the caller's responsibility to ensure that @code(newElement) points
      to an item that is at least @link(AVector.elementSize) bytes in
      length.  The value located there will be copied to the new element at
      the end of the array.  If @code(newElement) is @nil, then the new item
      is initialized to all zero (0) bytes.
    }
    procedure pushElement(const newElement: pointer);

    { Pop the last element from the array, thus removing it from the array.

      It is the caller's responsibility to ensure that @code(destElement)
      points to an item that is at least @link(AVector.elementSize) bytes
      in length.  The value located at the end of the array will be copied
      there.  If @code(destElement) is @nil, then the element is removed from
      the array and discarded.
    }
    procedure popElement(const destElement: pointer); virtual;

    { Add the element into the array as the last item in the array.

      If necessary, the array will grow to accomodate the new item.

      It is the caller's responsibility to ensure that @code(newElement) points
      to an item that is at least @link(AVector.elementSize) bytes in
      length.  The value located there will be copied to the new element at the
      end of the array.  If @code(newElement) is @nil, then the new item is
      initialized to all zero (0) bytes.

      @return(The index of the new item.  This index is not absolute; if
      items are removed from the array, then the indices of any items after the
      one removed will change.)
    }
    function appendElement(const newElement: pointer): longword; virtual;

    { Add the element into the array at the specified index.

      Any element which already exists at the index will be "shifted right"; in
      other words, the index assigned to it will increase by one.

      If @code(thisIndex) specifies an index that is larger than the current
      value of @link(AVector.length), then the item is added to the end
      of the array as though @link(AVector.pushElement) was called.

      It is the caller's responsibility to ensure that @code(newElement) points
      to an item that is at least @link(AVector.elementSize) bytes in
      length.  The value located there will be copied to the new element.  If
      @code(newElement) is @nil, then the new item is initialized to all zero
      (0) bytes.
    }
    procedure insertElementAt(thisIndex: longword; const newElement: pointer);
      virtual;

    { Remove the element at the specified index.

      Any and all elements which exist "after" the specified index will be
      "shifted left"; in other words, the indices assigned to them will decrease
      by one each.

      If @code(thisIndex) specifies an index that is larger than the current
      value of @link(AVector.length), then the last item is removed from
      the array as though @link(AVector.popElement) was called.

      It is the caller's responsibility to ensure that @code(destElement)
      points to an item that is at least @link(AVector.elementSize) bytes
      in length.  The value at the specified index will be copied there before
      it is removed from the array.  If @code(destElement) is @nil, then the
      element is removed from the array and discarded.
    }
    procedure removeElementAt(thisIndex: longword; const destElement: pointer);
      virtual;

    { Delete the element at the specified index.

      Any and all elements which exist "after" the specified index will be
      "shifted left"; in other words, the indices assigned to them will decrease
      by one each.

      If @code(thisIndex) specifies an index that is larger than the current
      value of @link(AVector.length), then the last item is removed from
      the array as though @link(AVector.popElement) was called.

      This method simply calls @link(AVector.removeElementAt), passing
      a @nil pointer for the @code(destElement).
    }
    procedure deleteElementAt(const thisIndex: longword); virtual;

    { Retrieve the element at the specified index.

      If @code(thisIndex) specifies an index that is larger than the current
      value of @link(AVector.length), then the value of the last item in
      the array is retrieved, exactly as though @link(AVector.lastElement)
      was called.

      It is the caller's responsibility to ensure that @code(destElement) points
      to an item that is at least @link(AVector.elementSize) bytes in
      length.  The value at the specified index will be copied there.  If
      @code(destElement) is @nil, then this routine does nothing.
    }
    procedure getElementAt(thisIndex: longword; const destElement: pointer);
      virtual;

    { Set the element at the specified index.

      If @code(thisIndex) specifies an index that is larger than the current
      value of @link(AVector.length), then this method sets the value of
      the last item in the array.

      It is the caller's responsiblity to ensure that @code(newElement) points
      to an item that is at least @link(AVector.elementSize) bytes in
      length.  The value specified there will be copied to the array at the
      specified index, overwriting any value that is already there.

      If @code(newElement) is @nil, then this routine zero-fills the element
      at the specified index.
    }
    procedure setElementAt(thisIndex: longword; const newElement: pointer);
      virtual;

    // CanStream implementation ------------------------------------------------
    { Write the array and its elements to the specified stream.

      This method writes the values of @link(AVector.elementSize) and
      @link(AVector.length) to @code(Dest) before writing the contents
      of the array itself.  Only the used contents of its buffer are written;
      any extra space allocated by @link(AVector.grow) is ignored.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeTo(const Dest: AStream): TStreamIOSize; virtual;

    { Read the array and its elements from the specified stream.

      This method reads the values of @link(AVector.elementSize) and
      @link(AVector.length) from @code(Source) before reading the contents
      of the array itself.  If the array already has elements, the new elements
      read from @code(Source) are appended to the end of the array, and the
      array is grown as necessary.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; virtual;

    // CanPrint implementation -------------------------------------------------
    { Construct a string representation of the object, suitable for output to a
      text-based device, such as a console.

      This method constructs a string representation based on the format
      specified by @link(vctrStringRepresentationSingular) or
      @link(vctrStringRepresentationPlural).  The format includes the
      name of the class and the total number of elements contained by the array.
    }
    function toString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the buffer used by the array.

      The calling routine should not make any assumptions about the pointer
      returned by this routine.  If it becomes necessary to resize the array,
      it is possible that this pointer will become invalid, depending upon how
      the underlying memory manager reallocates memory.

      The pointer returned by this routine should NOT be used to free the
      buffer managed by the array.
    }
    function buffer: pointer; virtual;

    { Retrieve a reference to the last item in the array.

      The pointer returned by this routine should NOT be freed.
    }
    function lastElement: pointer; virtual;

    { Retrieve the size, in bytes, of the buffer used by the array. }
    function bufferLength: longword; virtual;

    { Retrieve the size of, in bytes, of elements stored in the array. }
    function elementSize: longword; virtual;

    { Retrieve the total number of elements in the array. }
    function length: longword; virtual;
end;

{ An instance of this exception class is raised whenever there is a fatal error
  within an instance of @link(AVector).
}
type AVectorError = class(AnException);

{ An instance of this exception class is raised when allocating memory for the
  buffer fails in a call to @link(AVector.allocateElements).
}
type AVectorAllocationError = class(AVectorError);
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string controls the string representation created by a call to
    @link(AVector.toString).  This particular format is used
    when there is exactly one item in the array.

    The string placeholder is filled with the display name of the class,
    as returned by a call to @link(APrintingObject.displayName).  The
    integer placeholder is filled with the total number of elements in the
    managed array, as returned by a call to @link(AVector.length).  This
    value will always be one (1) when this string is used.
  }
  vctrStringRepresentationSingular = '%s that contains %d item';

  { This string controls the string representation created by a call to
    @link(AVector.toString).  This particular format is used when there
    are zero (0) or more than 1 (1) items in the array.

    The string placeholder is filled with the display name of the class, as
    returned by a call to @link(APrintingObject.displayName).  The integer
    placeholder is filled with the total number of elements in the managed
    array, as returned by a call to @link(AVector.length).
  }
  vctrStringRepresentationPlural = '%s that contains %d items';

  { This string controls the format of the error message passed to an instance
    of @link(AVectorAllocationError) when a call to
    @link(AVector.ofElements) or @link(AVector.grow) fails to
    allocate memory for the array.

    The integer placeholder is replaced with the total size of memory requested
    from the system, in bytes.  The string placeholder is filled with the
    display name of the class, as obtained by a call to
    @link(APrintingObject.displayName).
  }
  vctrErrorAllocatingMemory = 'failed to allocate %d bytes to hold elements ' +
    'of %s';
{$ENDIF}
{$IFDEF ReadImplementation}
// AVector *********************************************************************
// Constructor
constructor AVector.ofLength(const initialLength: longword;
  const elementsSize: longword; const initialValue: pointer = nil);

begin
  inherited new;

  // Do nothing if we have invalid parameters
  if (elementsSize = 0) or (initialLength = 0) then
    exit;


  // Attempt to allocate the buffer
  myElementSize := elementsSize;
  Self.allocateElements(initialLength);

  // Fill the array
  Self.fillElements(initialValue);
end;

// Initializer
function AVector.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myElementSize := 0;
    myLength := 0;
    myBuffer := nil;
    myBufferLength := 0;
  end;
end;

// Destructor
destructor AVector.destroy;

begin
  // Free the buffer
  if myBuffer <> nil then
    System.freeMem(myBuffer);
end;

// Allocate memory for the specified number of elements
procedure AVector.allocateElements(const elementCount: longword);

var
  // Points to the new buffer
  newBuffer: pointer;
  // Stores the size of the new buffer
  newBufferSize: longword;

begin
  // We do nothing if no additional elements are requested
  if (elementCount = 0) or (elementCount <= myLength) then
    exit;

  // Calculate the size of the new buffer
  newBufferSize := elementCount * myElementSize;

  newBuffer := System.reallocMem(myBuffer, newBufferSize);

  if newBuffer = nil then
    raise AVectorAllocationError.because(vctrErrorAllocatingMemory, [
      newBufferSize, Self.displayName
    ]);

  myBuffer := newBuffer;
  myBufferLength:= newBufferSize;
  myLength := elementCount;
end;

// Grow the array
procedure AVector.grow(newElementCount: longword = 0);

var
  // Stores the previous length of the array
  oldLength: longword;

begin
  if myBuffer = nil then
    exit;

  oldLength := myLength;

  { If no element count was specified, then grow the array by 1.5 times its
    current size
  }
  if newElementCount = 0 then
    newElementCount := myLength div 2;

  inc(newElementCount, myLength);

  Self.allocateElements(newElementCount);

  { Zero-fill the new elements }
  MemorySpan.fill(@myBuffer[oldLength], (newElementCount * myElementSize), 0);
end;

// Fill the array with copies of the given value
procedure AVector.fillElements(const sourceValue: pointer);

var
  // Loop counter
  i: longword;
  // Refers to the part of the buffer into which values are copied
  insertionPoint: pointer;

begin
  if myBuffer = nil then
    exit;

  // If a value was specified, then fill using copies of the value
  if sourceValue <> nil then
  begin
    // Loop through each element
    insertionPoint := myBuffer;
    for i := 1 to myLength do
    begin
      MemorySpan.copy(sourceValue, insertionPoint, myElementSize);
      inc(insertionPoint, myElementSize);
    end;
  end

  // Otherwise, zero-fill the buffer
  else
    MemorySpan.fill(myBuffer, myBufferLength, 0);
end;

// Push the element onto the array
procedure AVector.pushElement(const newElement: pointer);

var
  // Refers to the part of the buffer into which values are copied
  insertionPoint: pointer;

begin
  if (myBuffer = nil) or (newElement = nil) then
    exit;

  { Do we need to grow the array?  We do if adding another element will push
    us beyond the end of the buffer.
  }
  insertionPoint := Self.lastElement + myElementSize;

  if insertionPoint > @myBuffer[myBufferLength] then
  begin
    Self.grow;

    { Growing the array may move the buffer so we must reacquire the insertion
      point as the previous pointer may be invalid.
    }
    insertionPoint := Self.lastElement + myElementSize;
  end;

  // Copy the value
  MemorySpan.copy(newElement, insertionPoint, myElementSize);
  inc(myLength);
end;

// Pop an element from the array
procedure AVector.popElement(const destElement: pointer);

var
  // Refers to the part of the buffer from which values are copied
  deletionPoint: pointer;

begin
  if (myBuffer = nil) or (myLength = 0) or (destElement = nil) then
    exit;

  // Copy the value
  MemorySpan.copy(Self.lastElement, destElement, myElementSize);
  dec(myLength);
end;

// Append an element at the end of the array
function AVector.appendElement(const newElement: pointer): longword;

begin
  result := 0;

  if (myBuffer = nil) or (newElement = nil) then
    exit;

  Self.pushElement(newElement);
  result := myLength - 1;
end;

// Add an element at the specified index
procedure AVector.insertElementAt(thisIndex: longword;
  const newElement: pointer);

var
  // Refers to the part of the buffer into which values are copied
  insertionPoint: pointer;
  // Loop counter
  i: longword;

begin
  if (myBuffer = nil) or (newElement = nil) then
    exit;

  // Any index out of bounds simply adds to the end of the array
  if thisIndex > myLength then
    thisIndex := myLength;

  { Do we need to grow the array?  We do if adding another element will push
    us beyond the end of the buffer.
  }
  insertionPoint := Self.lastElement + myElementSize;
  if insertionPoint > @myBuffer[myBufferLength] then
    Self.grow;

  { Start copying from the end of the array down to the insertion point,
    shifting all elements to the "right"
  }
  insertionPoint := Self.lastElement + myElementSize;
  for i := myLength downto thisIndex do
  begin
    MemorySpan.copy((insertionPoint - myElementSize), insertionPoint,
      myElementSize);
    dec(insertionPoint, myElementSize);
  end;

  { Copy the new value }
  MemorySpan.copy(newElement, insertionPoint, myElementSize);
  inc(myLength);
end;

// Remove an element at the specified index
// Add an element at the specified index
procedure AVector.removeElementAt(thisIndex: longword;
  const destElement: pointer);

var
  // Refers to the part of the buffer into which values are copied
  insertionPoint: pointer;
  // Loop counter
  i: longword;

begin
  if myBuffer = nil then
    exit;

  // Any index out of bounds simply removes from the end of the array
  if thisIndex >= myLength then
    thisIndex := myLength - 1;

  // Copy the value
  insertionPoint := @myBuffer[thisIndex * myElementSize];
  if destElement <> nil then
    MemorySpan.copy(insertionPoint, destElement, myElementSize);

  { Start copying from the insertion point to the end of the array, shifting
    all elements to the "left"
  }
  for i := thisIndex to (myLength - 1) do
  begin
    MemorySpan.copy((insertionPoint + myElementSize), insertionPoint,
      myElementSize);
    inc(insertionPoint, myElementSize);
  end;

  dec(myLength);
end;

// Delete the element at the specified index
procedure AVector.deleteElementAt(const thisIndex: longword);

begin
  Self.removeElementAt(thisIndex, nil);
end;

// Retrieve the element at the specified index
procedure AVector.getElementAt(thisIndex: longword;
  const destElement: pointer);

begin
  // We do nothing if there is nowhere to put the element requested
  if destElement = nil then
    exit;

  // Correct attempts to copy beyond the bounds of the array
  if thisIndex >= myLength then
    thisIndex := myLength - 1;

  // Copy the item
  MemorySpan.copy(@myBuffer[thisIndex * myElementSize], destElement,
    myElementSize);
end;

// Set the element at the specified index
procedure AVector.setElementAt(thisIndex: longword;
  const newElement: pointer);

begin
  // Correct attempts to work beyond the bounds of the array
  if thisIndex >= myLength then
    thisIndex := myLength - 1;

  // Copy the new value, if one is provided
  if newElement <> nil then
    MemorySpan.copy(newElement, @myBuffer[thisIndex * myElementSize],
      myElementSize)

  // Otherwise, zero the value
  else
    MemorySpan.fill(@myBuffer[thisIndex * myElementSize], myElementSize, 0);
end;

// CanStream implementation ----------------------------------------------------
// Write the array and its contents to the specified stream
function AVector.writeTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest = nil then
    exit;

  // Write the number of elements
  result := Dest.write(sizeof(myLength), myLength);
  // Write the element size
  result := result + Dest.write(sizeof(myElementSize), myElementSize);

  // Write the contents of the buffer
  if myBuffer <> nil then
    result := result + Dest.write((myLength * myElementSize), myBuffer^);
end;

// Read the array and its contents from the specified stream
function AVector.readFrom(const Source: AStream): TStreamIOSize;

var
  // Stores the number of elements to read from the stream
  streamElementCount: longword;
  // Stores the size of elements to read from the stream
  streamElementSize: longword;
  // Stores the insertion index for items read from the stream
  streamElementIndex: longword;

begin
  result := 0;

  if Source = nil then
    exit;

  streamElementCount := 0;
  streamElementSize := 0;

  // Read the number of elements
  result := Source.read(sizeof(streamElementCount), streamElementCount);
  // Read the element size
  result := result + Source.read(sizeof(streamElementSize), streamElementSize);

  // Read the contents of the buffer
  if streamElementCount > 0 then
  begin
    streamElementIndex := myLength;

    // Grow the array
    Self.grow(streamElementCount);
    // Read the items
    result := result + Source.read((streamElementCount * streamElementSize),
      myBuffer[streamElementIndex * myElementSize]);
  end;
end;

// CanPrint implementation -----------------------------------------------------
// Construct a string representation of the array
function AVector.toString: AnsiString;

begin
  if myLength <> 1 then
    result := SysUtils.format(vctrStringRepresentationPlural, [
      Self.displayName, myLength
    ])

  else
    result := SysUtils.format(vctrStringRepresentationSingular, [
      Self.displayName, myLength
    ]);
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the raw buffer in which array elements are stored
function AVector.buffer: pointer;

begin
  result := myBuffer;
end;

// Retrieve a pointer to the last element in the array
function AVector.lastElement: pointer;

begin
  result := nil;

  if myBuffer <> nil then
    // Remember that arrays are zero-indexed
    result := @myBuffer[(myLength - 1) * myElementSize];
end;

// Retrieve the size of the buffer used by the array
function AVector.bufferLength: longword;

begin
  result := myBufferLength;
end;

// Retrieve the size of each element in the array
function AVector.elementSize: longword;

begin
  result := myElementSize;
end;

// Retrieve the number of elements in the array
function AVector.length: longword;

begin
  result := myLength;
end;

{$ENDIF}

