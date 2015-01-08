{ classwork.inc - A root class with extra functionality.

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
{ This class extends @code(TObject) to provide some additional functionality
  reminiscent of that provided by the base @code(Object) class in Objective
  C.
}
type AnObject = class(TObject)
  public
    { Create and initialize new instance of the object.

      This method is called after the instance is constructed.  It automatically
      calls @link(AnObject.init) to perform initialization of the object.  If
      @link(AnObject.init) returns @false, indicating that initialization
      failed, then this routine will throw an exception.

      @raises(AnException if @link(AnObject.init) returns @false when called
      from this method.)
    }
    constructor new; virtual;

    { Create a new instance of the object.

      This method is called after the instance is constructed.  It does not
      call @link(AnObject.init) to perform initialization of the new instance.
      In fact, in the base implementation, it does nothing.  Descendant classes
      may override this method -- but probably won't.
    }
    constructor alloc; virtual;

    { Construct a shallow copy of the given object.

      This method calls @code(TObject.ClassType) on @code(Other), then uses
      the class reference returned to construct a new instance that is of the
      same type as @code(Other).  It then calls @link(AnObject.shallowCopyFrom)
      on the new instance.

      If @code(Other) is @nil, this method does nothing and returns @nil.

      @raises(AnException if @link(AnObject.init) returns @false when called
      from this method.)
    }
    class function shallowCopyOf(const Other: AnObject): AnObject; virtual;

    { Initialize a new instance of the class.

      This method will be overridden by descendants of @link(AnObject), where it
      will be used to initialize the various members of the class.  If one or
      more of these methods cannot be initialized for some reason, the method
      may return @false to indicate failure.  If the method was called by
      @link(AnObject.new), then an exception will be raised because of this
      failure.

      The default implementation of this method always returns @true.
    }
    function init: boolean; virtual;

    { Destroy the instance.

      This method is automatically called by @code(TObject.free) and need
      never be called directly.  It is included here for the sake of
      completeness.
    }
    destructor destroy; override;

    { Determine whether the class is an instance of the one specified.

      Internally, this method simply uses the @code(is) operator to determine
      whether the present instance is related to the specified
      @code(ClassObject).  It will return @true if the instance is a descendant
      of @code(ClassObject) or one of @code(ClassObject)'s descendants;
      otherwise, it returns @false.
    }
    function isKindOf(ClassObject: TClass): boolean; virtual;

    { Determine whether the class is an instance of the one specified.

      This method allows you to determine whether the current instance is
      related to the one specified by specifying the name of the class as a
      string rather than a type.  Internally, the method searches backwards
      through the class hierarchy, beginning with the present class instance.
      If one of the parent classes encountered has @code(thisClassName), then
      the method will return @true.  Otherwise, it returns @false.
    }
    class function isKindOf(thisClassName: string): boolean; virtual;

    { Determine whether the class implements the given method.

      This method simply calls @code(TObject.methodAddress) to determine whether
      the method table contains a valid method with @code(thisMethodName).  If
      so, it returns @true; otherwise, it returns @false.
    }
    class function respondsTo(thisMethodName: string): boolean; virtual;

    { Determine whether the class implements the given interface.

      This method allows you to determine whether the current instance is
      part of a class that implements a given interface.  Internally, the
      method searches backwards through the class hierarchy, beginning with
      the current instance, and searches for @code(interfaceName) within the
      interface table of each class.  If @code(interfaceName) is found, then
      the method returns @true; otherwise, it returns @false.

      @code(interfaceName) will represent the canonical name of the interface,
      if you are compiling code using Free Pascal's @code(INTERFACES CORBA)
      directive; otherwise, it must be the GUID of the interface that is sought.
    }
    class function implements(interfaceName: string): boolean; virtual;

    { Make a shallow copy of the data from @code(Other), assigning it to the
      current object.

      The base implementation of this method does nothing except compare whether
      @code(Self) and @code(Other) are compatible.  Descendant classes may
      override this method to allow certain data to be copied between object
      instances.

      @return(@true if the data was copied successfully; @false if not.  Both
      @code(Self) and @code(Other) must be descendants of the same class in
      order for the copy to succeed.)
    }
    function shallowCopyFrom(const Other: AnObject): boolean; virtual;

    // Properties --------------------------------------------------------------
    { Retrieves the name of the class.

      The base implementation of this method simply calls
      @code(TObject.className) and returns the resulting string.  Descendant
      classes may override this method to produce different results; for
      instance, AFileStream overrides it to return the name of the file
      being streamed.
    }
    function name: string; virtual;
end;

{ This definition simplifies the implementation of @link(AnObjectVector), since
  it allows the base implementation of @link(AnObjectVector) to construct
  instances of a given object on the fly (such as when
  @link(AnObjectVector.readFrom) is called.
}
type AnObjectClass = class of AnObject;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This is the format of the error message passed to the exception raised when
    @link(AnObject.init) returns @false.

    The string placeholder will be filled by the name of the class that raised
    the exception, as returned by a call to @code(TObject.className).
  }
  objErrorInitializing = 'failed to create and initialize an instance of "%s"';
{$ENDIF}
{$IFDEF ReadImplementation}
// AnObject ********************************************************************
// Construct and initialize the instance
constructor AnObject.new;

begin
  inherited create;

  if Self.init = false then
    raise AnException.because(objErrorInitializing, [Self.name]);
end;

// Construct the instance
constructor AnObject.alloc;

begin
  inherited create;
end;

// Construct a shallow copy of the given instance
class function AnObject.shallowCopyOf(const Other: AnObject): AnObject;

var
  // Refers to the class of the object being copied
  OtherClass: AnObjectClass;

begin
  Result := nil;

  // We cannot make a copy of nothing
  if Other = nil then
    exit;

  // Construct a new instance
  OtherClass := AnObjectClass(Other.ClassType);
  Result := OtherClass.new;

  if Result <> nil then
    Result.shallowCopyFrom(Other);
end;

// Initialize the instance
function AnObject.init: boolean;

begin
  result := true;
end;

// Destroy the instance
destructor AnObject.destroy;

begin
  inherited destroy;
end;

// Determine if the instance is related to the specified class
function AnObject.isKindOf(ClassObject: TClass): boolean;

begin
  result := Self is ClassObject;
end;

// Determine if the instance is related to the named class
class function AnObject.isKindOf(thisClassName: string): boolean;

var
  // Stores a class reference
  ThisClass: TClass;

begin
  result := false;
  thisClass := Self;

  // Search backwards through the class hierarchy
  while assigned(ThisClass) do
  begin
    if thisClass.className = thisClassName then
    begin
      result := true;
      break;
    end;
    thisClass := thisClass.classType;
  end;
end;

// Determine if the instance has the named method
class function AnObject.respondsTo(thisMethodName: string): boolean;

begin
  result := Self.methodAddress(thisMethodName) <> nil;
end;

// Determine if the instance implements the named interface
class function AnObject.implements(interfaceName: string): boolean;

var
  // Points to the interface table
  pInterfaces: PInterfaceTable;
  // Loop counters
  interfaceCount, I: integer;

begin
  result := false;

  if interfaceName <> '' then
  begin
    // Get the interface table
    pInterfaces := Self.getInterfaceTable;

    if assigned(pInterfaces) then
    begin
      interfaceCount := pInterfaces^.entryCount - 1;

      // Search the interface table
      for I := 0 to interfaceCount do
        // See if a matching interface exists
        if pInterfaces^.entries[i].iidStr^ = interfaceName then
        begin
          result := true;
          break;
        end;
    end;
  end;
end;

// Make a shallow copy of the given object
function AnObject.shallowCopyFrom(const Other: AnObject): boolean;

begin
  result := false;

  if Other is Self.classType then
    result := true;
end;

// Properties ------------------------------------------------------------------
// Retrieve the name of the instance
function AnObject.name: string;

begin
  result := Self.className;
end;
{$ENDIF}

