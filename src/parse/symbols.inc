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

{ This type represents the storage type of a signed integer constant when it
  is encountered in a source stream.  Instances of
  @link(ASignedIntegerConstantSymbol) use a variable of this type to store a
  numeric representation of the symbol.

  This type is defined to make parser code more flexible.
}
type TSignedIntengerContant = int64;

{ This type represents the storage type of an unsigned integer constant when it
  is encountered in a source stream.  Instances of
  @link(AnUnsignedIntegerConstantSymbol) use a variable of this type to store a
  numeric representation of the symbol.

  This type is defined to make parser code more flexible.
}
type TUnsignedIntegerConstant = qword;

{ This type represents the storage type of a floating-point constant when it is
  encountered in a source stream.  Instances of
  @link(AFloatingPointConstantSymbol) use a variable of this type to store a
  numeric representation of the symbol.

  This type is defined to make parser code more flexible.
}
type TFloatingPointConstant = double;

{ This type defines the location of a symbol: both its scope (the symbol table
  to which it belongs) and its index within that table.  It is returned by a
  call to @link(ASymbol.reference).

  When symbols are written to a stream, their position within their symbol table
  is recorded by writing the scope -- the index of their symbol table within the
  master list of symbol tables -- and index within that table.

  When symbols are read from a binary stream, such as an intermediate code
  stream, it is assumed that parsing has already been done and so there is no
  further need to look up symbols by name.  Thus, symbol tables are not
  reconstituted as a dictionary; instead, they are generally read into an array
  (an instance of @link(ASymbolList), where each symbol is sorted based on its
  index.  Each array of symbols is, in turned, placed in an array.  Thus this
  definition may be thought of as the absolute address of a given symbol.

  All symbol references written to a binary stream are usually converted to
  scope:index references; this is done automatically by @link(AnIdentifierToken)
  and its descendants when @link(AnIdentifierToken.writeSelfTo) is called.
}
type TSymbolReference = record
  { Refers to the scope of the symbol (the symbol table to which it belongs). }
  scope: TSymbolScope;
  { Refers to the index of the symbol (its position within the symbol table). }
  index: longword;
end;

const
  { This constant enumerates one of the more common symbol table scopes.

    This constant represents a symbol that has no scope (belongs to no
    symbol table).
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
    but which has no definition (an undeclared identifier of some kind).
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
{ This class represents a symbol that is parsed from a source stream.  It
  indicates the symbol scope (the symbol table to which the symbol belongs),
  as well as the name of the source and line number where the symbol was
  declared.

  You will likely not directly instantiate this class, as it is meant to serve
  as a template for more specific symbol types, such as @link(ATypeIdentifier).
}
type ASymbol = class(AStringLeaf)
  protected
    { Refers to the symbol table to which the symbol belongs }
    myScope: TSymbolScope;
    { Indicates how the symbol was defined }
    myCategory: longword;
    { Refers to the symbol's parent type }
    myParentType: TSymbolReference;

  public
    { Construct a new symbol with the given name and type.

      This method overrides the behavior inherited from @link(AStringLeaf).
      It calls the inherited constructor, then associates the new symbol with
      the given @code(ParentType) by queriying @code(ParentType) for its
      scope and index.

      @code(ParentType) refers to an instance of @link(ASymbol) to which the
      new symbol belongs.  If the new symbol is a structured type that is not
      a class, then @code(ParentType) will likely be @nil.
    }
    constructor named(const thisKey: string;
      const ParentType: ASymbol); reintroduce; virtual;

    { Initializer }
    function init: boolean; override;

    { Retrieve a reference to the symbol, which contains the scope and index of
      the symbol.

      The reference returned by this routie can be used to locate the symbol
      after it has been retrieved from an intermediate code stream.
    }
    function reference: TSymbolReference; virtual;

    { Construct a shallow copy of the other object.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.shallowCopyFrom).  It calls that method, then checks to
      see whether @link(Other) is an instance of @link(ASymbol).  If so, it
      copies the values of
      @unorderedList(
        @item(@link(ASymbol.scope),)
        @item(@link(ASymbol.category), and)
        @item(@link(ASymbol.parentType))
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
      values of @link(ASymbol.category), and @link(ASymbol.parentType) to
      @code(Dest).

      @return(The total number of bytes written to the stream.)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the symbol from the specified stream.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.readFrom).  It calls that routine, then reads the values
      of @link(ASymbol.scope, @link(ASymbol.category) and
      @link(ASymbol.parentType) from @code(Source).

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // CanPrint implementation -------------------------------------------------
    { Construct a string representation of the object, suitable for output to
      a text-based device, such as a console.

      This method overrides the behavior inherited from @link(AStringLeaf).
      It returns a quoted form of the symbol name, as obtained by a call to
      @link(Charstring.quote) when applied to @link(AStringLeaf.name).
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

// ASimpleTypeIdentifier *******************************************************
{ This class represents a symbol which identifies a new (simple) type.  Simple
  type identifiers inherit from an existing type and add nothing new.
}
type ASimpleTypeIdentifier = class(ASymbol);

// ANumericConstantToken *******************************************************
{ This class represents a symbol which represents a numeric constant. }
type ANumericConstantSymbol = class(ASymbol);

// AnIntegerConstantToken ******************************************************
{ This class represents a symbol which represents an integer constant. }
type AnIntegerConstantSymbol = class(ANumericConstantSymbol);

// ASignedIntegerConstantToken *************************************************
{ This class represents a symbol which represents a signed integer constant. }
type ASignedIntegerConstantSymbol = class(AnIntegerConstantSymbol)
  protected
    { Stores the value of the symbol }
    myValue: TSignedIntegerConstant;

  public
    { Initializer }
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method builds upon the behavior inherited from
      @link(ASymbol.shallowCopyFrom).  It calls the inherited routine, then
      checks to see whether @code(Other) is an instance of
      @link(ASignedIntegerConstantSymbol).  If so, it copies the value of
      @link(ASignedIntegerConstantSymbol.value) from @code(Other) to
      @code(Self), overwriting the value in @code(Self).
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    { Write the symbol, and just the symbol, to the specified stream.

      This method builds upon the behavior inherited from
      @link(ASymbol.writeSelfTo).  It calls the inherited routine, then
      writes the value of @link(ASignedIntegerConstantSymbol.value) to the
      stream.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the symbol from the specified stream.

      This method builds upon the behavior inherited from
      @link(ASymbol.readFrom).  It calls the inherited routine, then
      reads the value of @link(ASignedIntegerConstantSymbol.value) from the
      stream.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // Properties --------------------------------------------------------------
    { Retrieve the value of the symbol. }
    function value: TSignedIntegerConstant; virtual;

    { Set the value of the symbol.

      @return(The previous value of the symbol.)
    }
    function setValue(const newValue: TSignedIntegerConstant): TSignedIntegerConstant;
      virtual;
end;

// AnUnsignedIntegerConstant ***************************************************
{ This class represents a symbol which represents an unsigned integer constant.
}
type AnUnsignedIntegerConstantSymbol = class(AnIntegerConstantSymbol)
  protected
    { Stores the value of the symbol }
    myValue: TUnsignedIntegerConstant;

  public
    { Initializer }
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method builds upon the behavior inherited from
      @link(ASymbol.shallowCopyFrom).  It calls the inherited routine, then
      checks to see whether @code(Other) is an instance of
      @link(AnUnsignedIntegerConstantSymbol).  If so, it copies the value of
      @link(AnUnsignedIntegerConstantSymbol.value) from @code(Other) to
      @code(Self), overwriting the value in @code(Self).
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    { Write the symbol, and just the symbol, to the specified stream.

      This method builds upon the behavior inherited from
      @link(ASymbol.writeSelfTo).  It calls the inherited routine, then
      writes the value of @link(AnUnsignedIntegerConstantSymbol.value) to the
      stream.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the symbol from the specified stream.

      This method builds upon the behavior inherited from
      @link(ASymbol.readFrom).  It calls the inherited routine, then
      reads the value of @link(AnUnsignedIntegerConstantSymbol.value) from the
      stream.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // Properties --------------------------------------------------------------
    { Retrieve the value of the symbol. }
    function value: TUnsignedIntegerConstant; virtual;

    { Set the value of the symbol.

      @return(The previous value of the symbol.)
    }
    function setValue(const newValue: TUnsignedIntegerConstant): TUnsignedIntegerConstant;
      virtual;
end;

// AFloatingPointConstant ******************************************************
{ This class represents a symbol which represents a floating-point constant. }
type AFloatingPointConstantSymbol = class(ANumericConstantSymbol)
  protected
    { Stores the value of the symbol }
    myValue: TFloatingPointConstant;

  public
    { Initializer }
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method builds upon the behavior inherited from
      @link(ASymbol.shallowCopyFrom).  It calls the inherited routine, then
      checks to see whether @code(Other) is an instance of
      @link(AFloatingPointConstantSymbol).  If so, it copies the value of
      @link(AFloatingPointConstantSymbol.value) from @code(Other) to
      @code(Self), overwriting the value in @code(Self).
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    { Write the symbol, and just the symbol, to the specified stream.

      This method builds upon the behavior inherited from
      @link(ASymbol.writeSelfTo).  It calls the inherited routine, then
      writes the value of @link(AFloatingPointConstantSymbol.value) to the
      stream.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the symbol from the specified stream.

      This method builds upon the behavior inherited from
      @link(ASymbol.readFrom).  It calls the inherited routine, then
      reads the value of @link(AFloatingPointConstantSymbol.value) from the
      stream.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // Properties --------------------------------------------------------------
    { Retrieve the value of the symbol. }
    function value: TFloatingPointConstant; virtual;

    { Set the value of the symbol.

      @return(The previous value of the symbol.)
    }
    function setValue(const newValue: TFloatingPointConstant): TFloatingPointConstant;
      virtual;
end;
