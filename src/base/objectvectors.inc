{ objectvectors.inc: A class to manage a dynamically-allocated block of
    memory that contains references to objects.

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
{ This class represents an array that contains references to objects:
  descendants of @link(AnObject).  Although it is assumed that all of the
  objects in the array will be of the same class, this is not required.

  As with its parent, instances of @link(AnObjectVector) can dynamically grow
  as needed and can write themselves to arbitrary streams.  The base
  implementation of @link(AnObjectVector) cannot successfully read its contents
  from a stream since the value of @link(AnObjectVector.ObjectType) refers to
  @link(AnObject).  Descendant classes that wish to make use of the base
  implementation of @link(AnObjectVector.readFrom) should override
  @link(AnObjectVector.init) and set the value of
  @link(AnObjectVector.ObjectType) to refer to the class of objects that will
  be read from the stream; alternatively, descendant classes may override
  @link(AnObjectVector.readFrom) to provide their own means of determining
  which classes to construct and insert into the vector.
}
type AnObjectVector = class(APointerVector)
  protected
    { Refers to the type of object managed by the vector }
    MyObjectType: AnObjectClass;
    { Indicates whether the vector owns the objects it contains }
    myOwnObjects: boolean;

  public
    { Initializer }
    function init: boolean; override;

    { Destroy the array and its contents.

      If @link(AnObjectVector.ownsObjects) is @true, then this method will
      call @code(TObject.free) on all valid pointers contained by the array.
      Otherwise, it is the caller's responsibility to maintain references to
      these objects so that they can be freed when they are no longer required.
    }
    destructor destroy; override;

    { Push an object onto the array.  The new object will be placed as the last
      element in the array.

      This method calls @link(APointerVector.pushReference).
    }
    procedure pushObject(const ThisObject: AnObject); virtual;

    { Append an object to the array.

      The new object will be placed as the last element in the array and its
      index will be returned.  However, the index returned is not guaranteed to
      remain valid; if objects are inserted into or removed from the array at
      indices that come before the index of the new object, then its index
      will change.

      This method calls @link(APointerVector.appendReference).
    }
    function appendObject(const ThisObject: AnObject): longword; virtual;

    { Pop the last object from the array and return it to the caller.

      If the array contains no objects, this method returns @nil.

      This method calls @link(APointerVector.popReference).
    }
    function PopObject: AnObject; virtual;

    { Insert the specified object at the specified index within the array.

      If an object already exists at @code(thisIndex), it will be "shifted
      right"; that is, its index (and the indices of all objects that come
      after it) will increase by one.  If @code(thisIndex) specifies a value
      that is greater than @code(AVector.length), then the new object will be
      added to the end of the array in much the same way as if
      @link(AnObjectVector.pushObject) were called.

      This method calls @link(APointerVector.insertReferenceAt).
    }
    procedure insertObjectAt(const thisIndex: longword;
      const ThisObject: AnObject); virtual;

    { Remove the object at the specified index and return it to the caller.

      The object is removed from the array and any objects that come after it
      will be "shifted left"; that is, their indices will each decrease by one.
      If @code(thisIndex) specifies a value that is greater than or equal to
      @link(AVector.length), then the last object in the array is removed and
      returned, exactly as if @link(AnObjectVector.PopObject) were called.

      This method calls @link(APointerVector.removeReferenceAt).
    }
    function RemoveObjectAt(const thisIndex: longword): AnObject; virtual;

    { Delete the object at the specified index.

      The object is removed from the array and freed with a call to
      @code(TObject.free).  Any objects that come after it will be "shifted
      left"; that is, their indices will each decrease by one.  If
      @code(thisIndex) specifies a value that is greater than or equal to
      @link(AVector.length), then the last object in the array is removed and
      freed.

      This method calls @link(APointerVector.removeReferenceAt).
    }
    procedure deleteObjectAt(const thisIndex: longword); virtual;

    { Retrieve the object at the specified index.

      The reference returned by this routine should not be freed without also
      setting the reference in the array to @nil; otherwise, the array will
      contain a reference to an invalid pointer that may cause your program to
      crash with a segmentation fault when the array is destroyed or when
      another routine attempts to make use of the same reference.

      If @code(thisIndex) specifies a value that is greater than or equal to
      @link(AVector.length), then the last object in the array is returned.  If
      the array has no objects, this method will return @nil.

      This method calls @link(APointerVector.referenceAt).
    }
    function ObjectAt(const thisIndex: longword): AnObject; virtual;

    { Set the object at the specified index.

      This method updates the object reference at @code(thisIndex) and returns
      the previous object reference that was stored there, if any.  If this
      method returns a value that is not @nil, it may be the caller's
      responsibility to ensure that the object referred to will be freed when it
      is no longer required.

      If @code(thisIndex) specifies a value that is greater than or equal to
      @link(AVector.length), then the last object in the array is altered.  If
      the array has no objects, then this method does nothing but return @nil.
    }
    function SetObjectAt(const thisIndex: longword;
      const ThisObject: AnObject): TObject; virtual;

    // CanStream implementation ------------------------------------------------
    { Calculate the number of bytes required to stream the vector.

      This method overrides the behavior inherited from
      @link(AVector.streamingLength): it queries the contents of the array to
      determine which objects support streaming and only those objects that
      implement @link(CanStream) will be included in the count.  It then adds
      the size of @link(AVector.length) and returns the final value to the
      caller.
    }
    function streamingLength: TStreamIOSize; override;

    { Write the array and its contents to the specified stream.

      This method overrides the behavior inherited from @link(AVector.writeTo):
      it first queries the contents of the array to determine which objects
      support streaming; only those objects that implement @link(CanStream) will
      be written to the stream.  Once the number of objects that support
      streaming is known, that number is written to the stream as the count of
      elements in the array; afterward, this method calls
      @link(CanStream.writeTo) on each object that supports it.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeTo(const Dest: AStream): TStreamIOSize; override;

    { Read the array and its contents from the specified stream.

      This method overrides the behavior inherited from @link(AVector.readFrom):
      it reads the number of elements from the stream, then constructs instances
      of @link(AnObjectVector.ObjectType) to read each element from the stream.
      These instances are then inserted into the vector.

      If @link(AnObjectVector.ObjectType) does not support the @link(CanStream)
      interface, then this method will only read the number of elements in the
      stream before returning.

      Descendant classes may override this method where more complex behavior
      is needed -- for instance, when there are many types of objects to be
      read from the stream.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // CanPrint implementation -------------------------------------------------
    { Print the array and its contents to the specified stream.

      This method overrides the behavior inherited from
      @link(APrintingObject.printTo): it first writes a string representation
      of the vector obtained by calling @link(AVector.toString).  It then
      queries each object it contains; for those objects that implement
      @link(CanPrint), it calls @link(CanPrint.printTo); for all other objects
      it simply prints the name of the class as obtained by a call to
      @code(TObject.className).

      @return(The total number of bytes prited to @code(Dest).)
    }
    function printTo(const Dest: AStream;
      prefix: AnsiString = '';
      suffix: AnsiString = ''): TStreamIOSize; override;

    // Properties --------------------------------------------------------------
    { Obtain a reference to the type of object that is managed by the vector.
      Strictly speaking, of course, the vector can manage any descendant of
      @link(AnObject) and they need not all be the same; however, in those cases
      where the objects contained by the vector @italic(are) all of the same
      type, this value allows the vector to construct instances of that type on
      the fly.
    }
    function ObjectType: AnObjectClass; virtual;

    { Determine whether or not the vector assumes ownership of the objects that
      it contains.

      If this value is @true, then the vector will attempt to free all valid
      (non-@nil) object references when it is destroyed, by calling
      @code(TObject.free) on each one.  Otherwise, these objects will NOT be
      freed when the vector is destroyed, and it is the responsibility of the
      caller to ensure that they are properly discarded when they are no longer
      required.
    }
    function ownsObjects: boolean; virtual;

    { Set whether or not the vector assumes ownership of the objects that it
      contains.

      If @code(flag) is set to @true, then the vector will attempt to free all
      valid (non-@nil) object references when it is destroyed, by calling
      @code(TObject.free) on each one.  If @code(flag) is set to @false, then
      these objects will NOT be freed when the vector is destroyed, and it is
      the responsiblity of the caller to ensure that this is done.

      @return(The previous value of @link(AnObjectVector.ownsObjects).)
    }
    function setOwnsObjects(const flag: boolean): boolean; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// A pointer to an instance of an object
type PAnObject = ^AnObject;

// Initializer
function AnObjectVector.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    // By default, we manage instances of AnObject
    MyObjectType := AnObject;
    // We do not automatically assume ownership of objects
    myOwnObjects := false;
  end;
end;

// Destructor
destructor AnObjectVector.destroy;

var
  // Points to an object reference
  MyObjects: PAnObject;
  // Loop counter
  i: longword;

begin
  // Free our objects, if we own them
  if (myBuffer <> nil) and (myLength > 0) and (myOwnObjects) then
  begin
    MyObjects := @myBuffer[0];

    for i := 0 to (myLength - 1) do
      if MyObjects[i] <> nil then
        MyObjects[i].free;
  end;

  // Call the inherited routine to destroy the buffer
  inherited destroy;
end;

// Push an object onto the end of the array
procedure AnObjectVector.pushObject(const ThisObject: AnObject);

begin
  Self.pushReference(ThisObject);
end;

// Append an object to the end of the array and return its index
function AnObjectVector.appendObject(const ThisObject: AnObject): longword;

begin
  result := Self.appendReference(ThisObject);
end;

// Pop an object from the end of the array
function AnObjectVector.PopObject: AnObject;

begin
  Result := AnObject(Self.popReference);
end;

// Insert an object into the array at the specified index
procedure AnObjectVector.insertObjectAt(const thisIndex: longword;
  const ThisObject: AnObject);

begin
  Self.insertReferenceAt(thisIndex, ThisObject);
end;

// Remove and return the object at the specified index
function AnObjectVector.RemoveObjectAt(const thisIndex: longword): AnObject;

begin
  Result := AnObject(Self.removeReferenceAt(thisIndex));
end;

// Delete the object at the specified index
procedure AnObjectVector.deleteObjectAt(const thisIndex: longword);

var
  // Refers to the object in question
  ThisObject: AnObject;

begin
  ThisObject := AnObject(Self.RemoveObjectAt(thisIndex));

  // Free the object
  if ThisObject <> nil then
    ThisObject.free;
end;

// Retrieve a reference to the object at the specified index
function AnObjectVector.ObjectAt(const thisIndex: longword): AnObject;

begin
  Result := AnObject(Self.referenceAt(thisIndex));
end;

// Set the object reference at the specified index
function AnObjectVector.SetObjectAt(const thisIndex: longword;
  const ThisObject: AnObject): TObject;

begin
  Result := AnObject(Self.setReferenceAt(thisIndex, ThisObject));
end;

// CanStream implementation ----------------------------------------------------
// Calculate the number of bytes required to stream the vector
function AnObjectVector.streamingLength: TStreamIOSize;

var
  // Refers to the list of objects
  MyObjects: PAnObject;
  // Loop counter
  i: longword;

begin
  result := sizeof(myLength);

  // Determine the number of objects that support streaming
  if (myBuffer <> nil) and (myLength > 0) then
  begin
    MyObjects := @myBuffer[0];

    for i := 0 to (myLength - 1) do
      if (MyObjects[i] <> nil) and (MyObjects[i] is CanStream) then
        with MyObjects[i] as CanStream do
          inc(result, streamingLength)
  end;
end;

// Write the array and its contents to the specified stream
function AnObjectVector.writeTo(const Dest: AStream): TStreamIOSize;

var
  // Stores the number of streaming objects that will be written to the stream
  streamingObjectCount: longword;
  // Refers to the list of objects
  MyObjects: PAnObject;
  // Loop counter
  i: longword;

begin
  result := 0;

  if Dest = nil then
    exit;

  streamingObjectCount := 0;

  // Determine the number of objects that support streaming
  if (myBuffer <> nil) and (myLength > 0) then
  begin
    MyObjects := @myBuffer[0];

    for i := 0 to (myLength - 1) do
      if (MyObjects[i] <> nil) and (MyObjects[i] is CanStream) then
        inc(streamingObjectCount);
  end;

  // Write the number of objects that support streaming
  result := Dest.write(sizeof(streamingObjectCount), streamingObjectCount);

  // Write each object
  if streamingObjectCount > 0 then
    for i := 0 to (myLength - 1) do
      if (MyObjects[i] <> nil) and (MyObjects[i] is CanStream) then
        with MyObjects[i] as CanStream do
          inc(result, writeTo(Dest));
end;

// Read the array and its contents from a stream -- does nothing here
function AnObjectVector.readFrom(const Source: AStream): TStreamIOSize;

var
  // Stores the number of elements in the stream
  streamObjectCount: longword;
  // Refers to a new object created from the stream
  NewObject: AnObject;

begin
  result := 0;

  if Source = nil then
    exit;

  // Read the number of objects that support streaming
  streamObjectCount := 0;
  result := Source.read(sizeof(streamObjectCount), streamObjectCount);

  // Read each object, but only if our object class supports streaming
  if MyObjectType.implements('CanStream') then
    while streamObjectCount > 0 do
    begin
      NewObject := MyObjectType.new;

      with NewObject as CanStream do
        result := result + readFrom(Source);

      Self.pushObject(NewObject);
    end;
end;

// CanPrint implementation -----------------------------------------------------
// Print the array and its contents to the specified stream
function AnObjectVector.printTo(const Dest: AStream;
  prefix: AnsiString = '';
  suffix: AnsiString = ''): TStreamIOSize;

var
  // Loop counter
  i: longword;
  // Refers to the array of objects
  MyObjects: PAnObject;

begin
  result := 0;

  if Dest = nil then
    exit;

  // First print a string representation of the class
  result := Dest.printEach([prefix, Self.toString, suffix]);

  // Print each object
  if (myBuffer <> nil) and (myLength > 0) then
  begin
    MyObjects := @myBuffer[0];

    for i := 0 to (myLength - 1) do
    begin
      // Check for a valid object
      if MyObjects[i] <> nil then
      begin
        // Does it support printing?
        if MyObjects[i] is CanPrint then
          with MyObjects[i] as CanPrint do
          result :=  result + printTo(Dest, prefix, suffix)

        // Otherwise, just print the class name
        else
          result :=
            result + Dest.printEach([prefix, MyObjects[i].className, suffix]);
      end;
    end;
  end;
end;

// Properties ------------------------------------------------------------------
// Retrieve the type of object managed by the vector
function AnObjectVector.ObjectType: AnObjectClass;

begin
  Result := MyObjectType;
end;

// Determine whether or not the vector owns its objects
function AnObjectVector.ownsObjects: boolean;

begin
  result := myOwnObjects;
end;

// Set whether or not the vector owns its objects
function AnObjectVector.setOwnsObjects(const flag: boolean): boolean;

begin
  result := myOwnObjects;
  myOwnObjects := flag;
end;

{$ENDIF}


