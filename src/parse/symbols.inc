{ symbols.inc - Classes to represent symbols (identifiers and constants) parsed
    from a source stream.

  This file is part of parsing.pp, a library of basic and useful parser classes.

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
{ This type represents the scope of a symbol, which is a way of indicating the
  symbol table to which it belongs.  Generally speaking, each program block has
  its own scope.

  This type is defined to make parser code more flexible.
}
type TSymbolScope = integer;

{ This type defines the location of a symbol: both its scope (the symbol table
  to which it belongs) and its index within that table.  It is returned by a
  call to @link(ASymbol.reference).

  When symbols are written to a stream, their position within their symbol table
  is recorded by writing the scope -- the index of their symbol table within the
  master list of symbol tables -- and index within that table.

  When symbols are read from a binary stream, such as an intermediate code
  stream, it is assumed that parsing has already been done and so there is no
  further need to look up symbols by name.  Thus, symbol tables are not normally
  reconstituted as a dictionary; instead, they are generally read into an array
  (an instance of @link(ASymbolVector), where each symbol is sorted based on its
  index.  Each array of symbols is, in turned, placed in an array.  Thus this
  definition may be thought of as the absolute address of a given symbol.

  All symbol references written to a binary stream are usually converted to
  @code(scope:index) references; this is done automatically by @link(ASymbol)
  and its descendants when @link(ASymbol.writeSelfTo) is called.
}
type TSymbolReference = record
  { Refers to the scope of the symbol (the symbol table to which it belongs). }
  scope: TSymbolScope;
  { Refers to the index of the symbol (its position within the symbol table). }
  index: integer;
end;

const
  { This constant enumerates one of the more common symbol table scopes.

    This constant represents a symbol that has no scope (belongs to no
    symbol table).  This is the default value for @link(ASymbol.scope) that is
    set by @link(ASymbol.init), although descendant classes may modify this
    value.
  }
  SYMSCOPE_NONE = -1;

  { This constant enumerates one of the more common symbol table scopes.

    This constant represents a symbol that belongs to the global symbol table
    (i.e., which is visible from everywhere in the source program).
  }
  SYMSCOPE_GLOBAL = 0;

const
  { This constant enumerates one of the ways in which a symbol may be defined in
    a source stream.

    This constant represents a symbol that was encountered in the source stream,
    but which has no definition (an undeclared identifier of some kind).  This
    is the default value for @link(ASymbol.category) that is set by
    @link(ASymbol.init), although descendant classes may modify this value.
  }
  SYMCAT_UNDEFINED = $00000000;

  { This constant enumerates one of the ways in which a symbol may be defined in
    a source stream.

    This constant represents a symbol that is declared as a data type.
  }
  SYMCAT_TYPE = $00000001;

  { This constant enumerates one of the ways in which a symbol may be defined in
    a source stream.

    This constant represents a symbol that is declared as a constant value:
    either a numeric constant or a string literal.
  }
  SYMCAT_LITERAL = $00000002;

  { This constant enumerates one of the ways in which a symbol may be defined in
    a source stream.

    This constant represents a symbol that is declared as a variable: an
    instance of a known type.
  }
  SYMCAT_VARIABLE = $00000003;

  { This constant enumerates one of the ways in which a symbol may be defined in
    a source stream.

    This constant represents a symbol that is declared as a subroutine: a
    function, procedure, or other labeled block of code.
  }
  SYMCAT_SUBROUTINE = $00000004;

  { This constant enumerates one of the ways in which a symbol may be defined in
    a source stream.

    This constant represents a symbol that is declared as a member of a
    structured type: a record, class, or other memory structure.
  }
  SYMCAT_STRUCTURE_MEMBER = $00000005;

  { This constant enumerates one of the ways in which a symbol may be defined in
    a source stream.

    This constant represents a symbol that is declared as a parameter accepted
    by a subroutine: one of the variables that may be passed to a subroutine.
  }
  SYMCAT_PARAMETER = $00000006;

  { This flag indicates that a symbol has been declared as external to the
    source.  It is designed to be combined with another symbol category
    code, such as @link(SYMCAT_SUBROUTINE) (to indicate an external function)
    or @link(SYMCAT_VARIABLE) (to indicate an external variable).
  }
  SYMCAT_EXTERNAL = $80000000;

// ASymbol *********************************************************************
{ This class represents a basic symbol, which may be an identifier (such as a
  variable or function name), a numeric constant, or a string literal.  It
  indicates the symbol scope (the symbol table to which the symbol belongs),
  as well as the name of the source and line number where the symbol was
  declared.

  You will likely not directly instantiate this class, as it is meant to serve
  as a template for more specific symbol types that may be required by a parser.
}
type ASymbol = class(AStringLeaf)
  protected
    { Refers to the symbol table to which the symbol belongs }
    myScope: TSymbolScope;
    { Indicates how the symbol was defined }
    myCategory: longword;

  public
    { Initializer }
    function init: boolean; override;

    { Retrieve a reference to the symbol, which contains the scope and index of
      the symbol.

      The reference returned by this routine can be used to locate the symbol
      after it has been retrieved from an intermediate code stream.  In a manner
      of speaking, this reference is the absolute address of the symbol, at
      which it can always be found (unless the source is modified).
    }
    function reference: TSymbolReference; virtual;

    { Construct a shallow copy of the other object.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.shallowCopyFrom).  It calls that method, then checks to
      see whether @code(Other) is an instance of @link(ASymbol).  If so, it
      copies the values of
      @unorderedList(
        @item(@link(ASymbol.scope), and)
        @item(@link(ASymbol.category))
      )
      from @code(Other) to @code(Self), overwriting the values in @code(Self).

      Note that this method does NOT copy the left and right subtree of
      @code(Other) and so does not place @code(Self) inside the same symbol
      table as @code(Other), if any.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    { Write the symbol, and just the symbol, to the specified stream.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.writeSelfTo).  It calls that method, then writes the
      values of @link(ASymbol.category) and @link(ASymbol.scope).

      @return(The total number of bytes written to the stream.)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the symbol from the specified stream.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.readFrom).  It calls that routine, then reads the values
      of @link(ASymbol.scope) and @link(ASymbol.category) from @code(Source).

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // CanPrint implementation -------------------------------------------------
    { Construct a string representation of the object, suitable for output to
      a text-based device, such as a console.

      This method overrides the behavior inherited from @link(AStringLeaf).
      It returns a quoted form of the symbol name, as obtained by a call to
      @link(Charstring.quote) when applied to @link(AStringLeaf.name).  This
      behavior is designed to make it easy to output a symbol as part of a
      syntax error message, hint, or warning.
    }
    function toString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve the symbol's scope, which indicates the symbol table to which
      it belongs.
    }
    function scope: TSymbolScope; virtual;

    { Get the symbol category, which indicates how it was defined.

      The value returned by this routine will be one of the @code(SYMCAT_)
      constants, such as @link(SYMCAT_VARIABLE).
    }
    function category: longword; virtual;
end;

// ASymbolFromSource ***********************************************************
{ This class represents a symbol that is parsed from a source file and which
  will be handled at once or written to an intermediate code file.

  Instances of @link(ASymbolFromSource) contain a reference to their parent
  symbol, if any, which can be accessed by calling
  @link(ASymbolFromSource.ParentSymbol).  This is useful when a symbol
  represents a variable or some other source item that derives from a
  pre-defined type.  Although instances of @link(ASymbolFromSource) make use
  of the @code(Parent) property inherited from @link(ANode), symbols are not
  normally collected into hierarchical lists but are instead kept in either
  a tree or linear sequence to make symbol lookup as fast as possible.

  When @link(ASymbolFromSource.writeSelfTo) is called, the symbol will write
  an absolute reference to its parent, if any, to the destination stream.  This
  reference is an instance of @link(TSymbolReference) and so is independent of
  any pointer value, which may change between the time the parent symbol is
  written and the time the symbol is recalled from a stream.  Although it is
  possible to read instances of @link(ASymbolFromSource) from a binary stream,
  the base implementation of this class does not know how to read the parent
  reference.  You should use an instance of @link(ASymbolRecalled) instead.
}
type ASymbolFromSource = class(ASymbol)
  public
    { Construct a new symbol with the given name and type.

      This method overrides the behavior inherited from @link(AStringLeaf).
      It calls the inherited constructor, then associates the new symbol with
      the given @code(ParentType).

      @code(ParentType) refers to an instance of @link(ASymbol) to which the
      new symbol belongs.  If the new symbol is a structured type that is not
      a class, then @code(ParentType) will likely be @nil.
    }
    constructor named(const thisKey: string;
      const ParentType: ASymbol); reintroduce; virtual;

    { Construct a shallow copy of the other object.

      This method builds upon the behavior inherited from
      @link(ASymbol.shallowCopyFrom).  It calls that method, then checks to
      see whether @code(Other) is an instance of @link(ASymbolFromSource).  If
      so, it copies the value of @link(ASymbolFromSource.ParentSymbol) from
      @code(Other) to @code(Self), overwriting the value in @code(Self).

      Note that this method does NOT copy the left and right subtree of
      @code(Other) and so does not place @code(Self) inside the same symbol
      table as @code(Other), if any.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    { Write the symbol, and just the symbol, to the specified stream.

      This method builds upon the behavior inherited from
      @link(ASymbol.writeSelfTo).  It calls that method, then calls
      @link(ASymbol.reference) on its parent, if any, and writes those values,
      too.  This behavior is designed to make it easy to recall a symbol from
      an intermediate code stream.

      @return(The total number of bytes written to the stream.)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the parent symbol of the symbol, if any.  If the
      symbol has no parent (it is, itself, representative of a base type),
      then the return value from this routine will be @nil.

      The caller should NOT free the reference returned by this routine.
    }
    function ParentSymbol: ASymbol; virtual;
end;

// ASymbolRecalled *************************************************************
{ This class represents a symbol which was previously parsed and written to an
  intermediate code file.  Instead of providing a pointer reference to the
  parent, if any, instances of @link(ASymbolRecalled) utilize
  @link(TSymbolReference) to point to the scope and index of their parent.  This
  information is read from a stream, to which it was recorded by an instance of
  @link(ASymbolFromSource).
}
type ASymbolRecalled = class(ASymbol)
  protected
    { Refers to the parent symbol, if any }
    myParentSymbol: TSymbolReference;

  public
    { Initializer }
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method builds upon the behavior inherited from
      @link(ASymbol.shallowCopyFrom).  It calls that method, then checks to
      see whether @code(Other) is an instance of @link(ASymbolRecalled).  If
      so, it copies the value of @link(ASymbolRecalled.parentSymbol) from
      @code(Other) to @code(Self), overwriting the value in @code(Self).

      Note that this method does NOT copy the left and right subtree of
      @code(Other) and so does not place @code(Self) inside the same symbol
      table as @code(Other), if any.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    { Write the symbol, and just the symbol, to the specified stream.

      This method builds upon the behavior inherited from
      @link(ASymbol.writeSelfTo).  It calls that method, then writes the value
      of @link(ASymbolRecalled.parentSymbol).

      @return(The total number of bytes written to the stream.)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the symbol from the specified stream.

      This method builds upon the behavior inherited from
      @link(ASymbol.readFrom).  It calls that routine, then reads the value
      of @link(ASymbolRecalled.parentSymbol) from @code(Source).

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the parent type of the symbol, if any.  If the
      symbol has no parent type (it is, itself, representative of a base type),
      then the return value from this routine will be empty.
    }
    function parentSymbol: TSymbolReference; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// ASymbol *********************************************************************
// Initializer
function ASymbol.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myScope := SYMSCOPE_NONE;
    myCategory := SYMCAT_UNDEFINED;
  end;
end;

// Construct a symbol reference (absolute address) for the symbol
function ASymbol.reference: TSymbolReference;

begin
  result.scope := myScope;
  result.index := myIndex;
end;

// Construct a shallow copy of the other object
function ASymbol.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // If that succeeded, then...
  if (result) and (Other is ASymbol) then
  begin
    myScope := ASymbol(Other).scope;
    myCategory := ASymbol(Other).category;
  end;
end;

// Write the symbol to the given stream
function ASymbol.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest = nil then
    exit;

  // Call the inherited routine first
  result := inherited writeSelfTo(Dest);

  // Write our scope
  result := result + Dest.write(sizeof(myScope), myScope);
  // Write our category
  result := result + Dest.write(sizeof(myCategory), myCategory);
end;

// CanStream implementation ----------------------------------------------------
// Read the symbol from the given stream
function ASymbol.readFrom(const Source: AStream): TStreamIOSize;

begin
  result := 0;

  if Source = nil then
    exit;

  // Call the inherited routine first
  result := inherited readFrom(Source);

  // Read our scope
  result := result + Source.read(sizeof(myScope), myScope);
  // Read our category
  result := result + Source.read(sizeof(myCategory), myCategory);
end;

// CanPrint implementation -----------------------------------------------------
// Construct a string representation of the symbol
function ASymbol.toString: AnsiString;

begin
  result := Charstring.quote(Self.name);
end;

// Properties ------------------------------------------------------------------
// Retrieve the symbol scope
function ASymbol.scope: TSymbolScope;

begin
  result := myScope;
end;

// Retrieve the symbol category
function ASymbol.category: longword;

begin
  result := myCategory;
end;

// ASymbolFromSource ***********************************************************
// Constructor
constructor ASymbolFromSource.named(const thisKey: string;
  const ParentType: ASymbol);

begin
  inherited named(thisKey);

  MyParent := ParentType;
end;

// Construct a shallow copy of the other object
function ASymbolFromSource.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to ensure we're compatible with the other object
  if (result) and (Other is ASymbolFromSource) then
  begin
    MyParent := ASymbolFromSource(Other).Parent;
  end;
end;

// Write the symbol to the given stream
function ASymbolFromSource.writeSelfTo(const Dest: AStream): TStreamIOSize;

var
  // Stores a reference to the parent symbol, if any
  parentReference: TSymbolReference;

begin
  result := 0;

  if Dest = nil then
    exit;

  // Call the inherited routine first
  result := inherited writeSelfTo(Dest);

  // Write an absolute reference to our parent, if any
  if MyParent <> nil then
    parentReference := ASymbol(MyParent).reference

  else begin
    parentReference.scope := SYMSCOPE_NONE;
    parentReference.index := -1;
  end;

  result := result + Dest.write(sizeof(parentReference), parentReference);
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the parent symbol, if any
function ASymbolFromSource.ParentSymbol: ASymbol;

begin
  Result := ASymbol(MyParent);
end;

// ASymbolRecalled *************************************************************
// Initializer
function ASymbolRecalled.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myParentSymbol.scope := SYMSCOPE_NONE;
    myParentSymbol.index := -1;
  end;
end;

// Construct a shallow copy of the other object
function ASymbolRecalled.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Ensure we're compatible with the other object
  if (result) and (Other is ASymbolRecalled) then
  begin
    myParentSymbol := ASymbolRecalled(Other).parentSymbol;
  end;
end;

// Write the symbol to the given stream
function ASymbolRecalled.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest = nil then
    exit;

  // Call the inherited routine first
  result := inherited writeSelfTo(Dest);

  // Write an absolute reference to our parent
  result := result + Dest.write(sizeof(myParentSymbol), myParentSymbol);
end;

// CanStream implementation ----------------------------------------------------
// Read the symbol from the given stream
function ASymbolRecalled.readFrom(const Source: AStream): TStreamIOSize;

begin
  result := 0;

  if Source = nil then
    exit;

  // Call the inherited routine first
  result := inherited readFrom(Source);

  // Read the absolute reference to our parent
  result := result + Source.read(sizeof(myParentSymbol), myParentSymbol);
end;

// Properties ------------------------------------------------------------------
// Return a reference to the parent of this symbol, if any
function ASymbolRecalled.parentSymbol: TSymbolReference;

begin
  result := myParentSymbol;
end;

{$ENDIF}

