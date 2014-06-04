{ pointervectors.inc: A class to manage a dynamically-allocated block of
    memory that contains pointers.

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
{ This class represents a vector of pointers.  It is unlikely that you
  will directly instantiate this class, since its functionality is fairly
  basic.  Instead, you will either derive a class from it that implements
  the higher-level functionality required, or you can use an instance of
  @link(AnObjectVector).

  The size of the elements managed by this array will depend on the target
  platform for which the code is compiled.  On 32-bit systems, pointers
  generally occupy four bytes, whereas on 64-bit systems they take up eight
  bytes.

  It is worth noting that, although this class is capable of writing its data to
  a stream, and of retrieving it therefrom, it is unlikely that such behavior
  will be of much use because pointer values read from a stream are likely to be
  invalid unless some method is devised to apply a fixup to each pointer value
  or to ensure that the items referenced will always exist at the same location
  in memory.
}
type APointerVector = class(AVector)
  public
    { Construct a new managed array of pointers that will initially contain the
      specified number of elements.

      The elements are zero-filled by this routine.

      This method calls @link(AVector.ofLength) and so may raise an
      exception if a block of memory large enough to contain the requested
      number of elements could not be allocated.

      @raises(AVectorAllocationError if a block of memory large enough to
      hold @code(elementCount) could not be allocated.)
    }
    constructor ofLength(const elementCount: longword); reintroduce; virtual;

    { Push a pointer onto the end of the array.

      This method calls @link(AVector.pushElement), which will grow the
      array as needed.
    }
    procedure pushReference(const source: pointer); virtual;

    { Push a pointer to the given item onto the end of the array.

      This method creates a pointer to @code(source) and pushes it onto the
      array.

      This method calls @link(AVector.pushElement), which will grow the
      array as needed.
    }
    procedure pushReferenceTo(var source); virtual;

    { Add a pointer to the end of the array and return the index assigned to it.

      This method pushes the specified pointer onto the end of the array and
      returns its index.  This index is NOT guaranteed to remain static; if
      items are added to or removed from the array at positions that come before
      the new item, then the new item's index will change.

      This method calls @link(AVector.appendElement).
    }
    function appendReference(const source: pointer): longword; virtual;

    { Add a pointer to the given item to the end of the array and return the
      index assigned to it.

      This method pushes a pointer to @code(source) onto the end of the array
      and returns its index.  This index is NOT guaranteed to remain static; if
      items are added to or removed from the array at positions that come before
      the new item, then the new item's index will change.

      This method calls @link(AVector.appendElement).
    }
    function appendReferenceTo(var source): longword; virtual;

    { Pop the last pointer from the end of the array.

      This method retrieves the last element in the array and returns it to the
      caller.  If the array has no elements, then it returns @nil.

      This method calls @link(AVector.popElement).
    }
    function popReference: pointer; virtual;

    { Insert the specified pointer at the specified index in the array.

      If an item already exists at @code(thisIndex), then it is "moved right":
      its index will increase by one, as will the indices of all items that
      come after it.  If @code(thisIndex) specifies a value that is greater than
      @link(AVector.length), then the @code(source) pointer is simply
      added to the end of the array in much the same way as if
      @link(APointerVector.pushReference) were called.

      This method calls @link(AVector.insertElementAt).
    }
    procedure insertReferenceAt(const thisIndex: longword;
      const source: pointer); virtual;

    { Insert a pointer to the specified item at the specified index in the
      array.

      If an item already exists @code(thisIndex), then it is "moved right": its
      index will increase by one, as will the indices of all items that come
      after it.  If @code(thisIndex) specifies a value that is greater than
      @link(AVector.length), then the pointer to @code(source) is simply
      placed at the end of the array in much the same way as if
      @link(APointerVector.pushReferenceTo) were called.

      This method calls @link(AVector.insertElementAt).
    }
    procedure insertReferenceTo(var source; const thisIndex: longword); virtual;

    { Remove and return the pointer at the specified index.

      This method removed the pointer at @code(thisIndex) and returns it to
      the caller.  Any items in the array which have higher indices than
      @code(thisIndex) are "moved left": their indices decrease by one.  If
      @code(thisIndex) specifies a value that is greater than
      @link(AVector.length), then this method simply returns the last
      pointer in the array in much the same way as if
      @link(APointerVector.popReference) were called.

      This method calls @link(AVector.removeElementAt).
    }
    function removeReferenceAt(const thisIndex: longword): pointer; virtual;

    { Delete the pointer at the specified index.

      This method deletes the pointer at @code(thisIndex).  Any items in the
      array which have higher indices than @code(thisIndex) are "moved left":
      their indices decrease by one.  If @code(thisIndex) specifies a value that
      is greater than @link(AVector.length), then this method removes the
      last pointer in the array.

      Note that this method does not free the item referenced by the pointer
      which is discarded; it is the caller's responsibility to ensure that
      this happens, if necessary, in order to avoid memory leaks.

      This method calls @link(AVector.deleteElementAt).
    }
    procedure deleteReferenceAt(const thisIndex: longword); virtual;

    { Retrieve the pointer at the specified index.

      This method retrieves the pointer at @code(thisIndex).  If
      @code(thisIndex) specifies a value that is greater than
      @link(AVector.length), then this method retrieves the last pointer
      in the array.

      The caller is free to manipulate the pointer as desired.

      This method calls @link(AVector.getElementAt).
    }
    function referenceAt(const thisIndex: longword): pointer; virtual;

    { Set the pointer at the specified index.

      This method sets the pointer at @code(thisIndex).  If
      @code(thisIndex) specifies a value that is greater than
      @link(AVector.length), then this method sets the last pointer in
      the array.

      This method calls @link(AVector.setElementAt).

      @return(The previous pointer at the specified index, if any.)
    }
    function setReferenceAt(const thisIndex: longword;
      const source: pointer): pointer; virtual;

    { Set the pointer at the specified index to point to the specified item.

      This method obtains a pointer to @code(source) and sets the pointer at
      @code(thisIndex) to that value.  If @code(thisIndex) specifies a value
      that is greater than @link(AVector.length), then this method sets
      the last pointer in the array.

      @return(The previous pointer at the specified index, if any.)
    }
    function setReferenceTo(var source; const thisIndex: longword): pointer;
      virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// Constructor
constructor APointerVector.ofLength(const elementCount: longword);

begin
  inherited ofLength(elementCount, sizeof(pointer));
end;

// Push the specified pointer onto the end of the array
procedure APointerVector.pushReference(const source: pointer);

begin
  Self.pushElement(@source);
end;

// Push a pointer to the specified item onto the end of the array
procedure APointerVector.pushReferenceTo(var source);

var
  // Stores a pointer to the specified source item
  sourceReference: pointer;

begin
  sourceReference := @source;

  Self.pushElement(@sourceReference);
end;

// Append a pointer to the array and retrieve its index
function APointerVector.appendReference(const source: pointer): longword;

begin
  result := Self.appendElement(@source);
end;

// Append a pointer to the specified item and retrieve its index
function APointerVector.appendReferenceTo(var source): longword;

var
  // Stores a pointer to the specified source item
  sourceReference: pointer;

begin
  sourceReference := @source;

  result := Self.appendElement(@sourceReference);
end;

// Pop the last pointer from the array
function APointerVector.popReference: pointer;

begin
  result := nil;

  Self.popElement(@result);
end;

// Insert a pointer at the specified index
procedure APointerVector.insertReferenceAt(const thisIndex: longword;
  const source: pointer);

begin
  Self.insertElementAt(thisIndex, @source);
end;

// Insert a pointer to the specified item at the specified index
procedure APointerVector.insertReferenceTo(var source;
  const thisIndex: longword);

var
  // Stores a reference to the specified source item
  sourceReference: pointer;

begin
  sourceReference := @source;

  Self.insertElementAt(thisIndex, @sourceReference);
end;

// Remove the pointer at the specified index
function APointerVector.removeReferenceAt(const thisIndex: longword): pointer;

begin
  result := nil;

  Self.removeElementAt(thisIndex, @result);
end;

// Delete the pointer at the specified index
procedure APointerVector.deleteReferenceAt(const thisIndex: longword);

begin
  Self.deleteElementAt(thisIndex);
end;

// Retrieve the pointer at the specified index
function APointerVector.referenceAt(const thisIndex: longword): pointer;

begin
  result := nil;

  Self.getElementAt(thisIndex, @result);
end;

// Set the pointer at the specified index
function APointerVector.setReferenceAt(const thisIndex: longword;
  const source: pointer): pointer;

begin
  result := nil;

  // First, get the value of the existing pointer so we can return it
  Self.getElementAt(thisIndex, @result);

  // Now, set the new value
  Self.setElementAt(thisIndex, @source);
end;

// Set the pointer at the specified index to point to the specified item
function APointerVector.setReferenceTo(var source;
  const thisIndex: longword): pointer;

var
  // Stores a reference to the specified source item
  sourceReference: pointer;

begin
  result := nil;

  // First, get the value of the existing pointer so we can return it
  Self.getElementAt(thisIndex, @result);

  // Now, set the new value
  sourceReference := @source;
  Self.setElementAt(thisIndex, @sourceReference);
end;
{$ENDIF}
